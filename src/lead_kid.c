#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include <gbdk/far_ptr.h>

#include "sfxplayer.h"
#include "hUGEDriver.h"
#include "lead_kid.h"
#include "levels.h"
#include "main.h"
#include "maps.h"
#include "sfx_data.h"

const metasprite_t META_LK_IDLE[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = S_FLIPX},
	METASPR_TERM
};

const metasprite_t META_LK_JUMP[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0b100, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = S_FLIPX},
	METASPR_TERM
};

const metasprite_t META_LK_WALK1[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = S_FLIPX},
	METASPR_TERM
};

const metasprite_t META_LK_WALK2[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = S_FLIPX},
	METASPR_TERM
};

uint8_t pos_x, pos_y, joy, joy_prev, lk_state, damage, jump_buf,
coyote_time, init_pos_x, init_pos_y, checkpoint_dmg;
int8_t vel_y;

void take_damage(void)
{
	PLAY_SOUND(SFX_DAMAGE);
	if (damage < UINT8_MAX)
		damage++;
	TREASURE_APPEAR_FALSE
	P_RANK_FALSE
	delay(0b100000000);

	pos_x = init_pos_x;
	pos_y = init_pos_y;
	vel_y = 0;

	just_paused = 0;
	init_map(cur_map);
}

inline uint16_t tile_index(uint8_t x, uint8_t y)
{
	return ((x >> 0b11) + ((y >> 0b11)*0b10100));
}

inline uint8_t is_solid(const uint8_t X, const uint8_t Y)
{
	return X < 0b10100000 && Y < 0b10010000 &&
	cur_map_ptr[tile_index(X, Y)] < NUM_SOLID_TILES;
}

void move_right(void)
{
	for (uint8_t i = 0; i < SPD_X; i++) {
		if (!is_solid(pos_x + 0b110, pos_y)
		&& !is_solid(pos_x + 0b110, pos_y - 0b110)
		&& !is_solid(pos_x + 0b110, pos_y + 0b101))
			pos_x++;
		else
			return;
	}
}

void move_left(void)
{
	for (uint8_t i = 0; i < SPD_X; i++) {
		if (!is_solid(pos_x - 0b111, pos_y)
		&& !is_solid(pos_x - 0b111, pos_y - 0b110)
		&& !is_solid(pos_x - 0b111, pos_y + 0b101))
			pos_x--;
		else
			return;
	}
}

void lead_kid_update(void)
{
	MOVING_FALSE;

	if ((cur_map_ptr[tile_index(pos_x, pos_y + 0b110)] == SPIKE_UP_TILE ||
	cur_map_ptr[tile_index(pos_x - 0b111, pos_y)] == SPIKE_LEFT_TILE ||
	cur_map_ptr[tile_index(pos_x, pos_y - 0b111)] == SPIKE_DOWN_TILE ||
	cur_map_ptr[tile_index(pos_x + 0b110, pos_y)] == SPIKE_RIGHT_TILE)) {
		TAKE_DAMAGE
	}

	if (JUMPING) {
		if ((is_solid(pos_x, pos_y - 0b111)
		|| is_solid(pos_x + 0b101, pos_y - 0b111)
		|| is_solid(pos_x - 0b110, pos_y - 0b111)) && vel_y < 1) {
			PLAY_SOUND(SFX_BONK);
			vel_y = ~(vel_y);
		}

		pos_y += vel_y;
		if (vel_y < 0b1111111)
			vel_y += ACL_Y;
	}

	if ((is_solid(pos_x, pos_y + 0b110 + vel_y)
	|| is_solid(pos_x + 0b101, pos_y + 0b110 + vel_y)
	|| is_solid(pos_x - 0b110, pos_y + 0b110 + vel_y))) {
		for (int8_t i = 1; i <= vel_y; i++) {
			if ((is_solid(pos_x, pos_y + 0b110 + i)
			|| is_solid(pos_x + 0b101, pos_y + 0b110 + i)
			|| is_solid(pos_x - 0b110, pos_y + 0b110 + i))) {
				pos_y += i;
				break;
			}
		}
		if ((cur_map_ptr[tile_index(pos_x, pos_y + 0b110)] == SPRING_TILE ||
		cur_map_ptr[tile_index(pos_x + 0b101, pos_y + 0b110)] == SPRING_TILE ||
		cur_map_ptr[tile_index(pos_x - 0b110, pos_y + 0b110)] == SPRING_TILE)
		&& vel_y > 0) {
			PLAY_SOUND(SFX_SPRING);

			vel_y = ~(vel_y);

		} else {
			JUMPING_FALSE;
			vel_y = 0;
			coyote_time = COYOTE_TIME_MAX;
		}
	} else if (coyote_time > 0) {
		coyote_time--;
	} else {
		JUMPING_TRUE;
	}

	if (joy & J_RIGHT) {
		MOVING_TRUE;
		if (FLIPPED)
			FLIPPED_FALSE;

		move_right();
	}
	if (joy & J_LEFT) {
		MOVING_TRUE;
		if (!(FLIPPED))
			FLIPPED_TRUE;

		move_left();
	}

	if (cur_map_ptr[tile_index(pos_x, pos_y + 0b110)] == CONVEYOR_LEFT_TILE ||
	cur_map_ptr[tile_index(pos_x + 0b101, pos_y + 0b110)] == CONVEYOR_LEFT_TILE ||
	cur_map_ptr[tile_index(pos_x - 0b110, pos_y + 0b110)] == CONVEYOR_LEFT_TILE)
		move_left();
	else if (cur_map_ptr[tile_index(pos_x, pos_y + 0b110)] == CONVEYOR_RIGHT_TILE ||
	cur_map_ptr[tile_index(pos_x + 0b101, pos_y + 0b110)] == CONVEYOR_RIGHT_TILE ||
	cur_map_ptr[tile_index(pos_x - 0b110, pos_y + 0b110)] == CONVEYOR_RIGHT_TILE)
		move_right();

	if (joy & (J_A | J_B | J_SELECT) && !(joy_prev & (J_A | J_B | J_SELECT))) {
		if (JUMPING) {
			jump_buf = JUMP_BUF_MAX;
		} else {
			PLAY_SOUND(SFX_JUMP);

			JUMPING_TRUE;
			vel_y = JUMP_HEIGHT;
		}
	}
	
	if (jump_buf > 0) {
		jump_buf--;

		if (!(JUMPING)) {
			PLAY_SOUND(SFX_JUMP);

			JUMPING_TRUE;
			vel_y = JUMP_HEIGHT;
		}
	}

	if (JUMPING) {
		FLIPPED ? move_metasprite_flipx(META_LK_JUMP, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000)
		: move_metasprite_ex(META_LK_JUMP, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000);

	} else if (MOVING && (sys_time & 0b1000)) {
		if (sys_time & 0b10000)
			FLIPPED ? move_metasprite_flipx(META_LK_WALK1, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000)
			: move_metasprite_ex(META_LK_WALK1, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000);

		else
			FLIPPED ? move_metasprite_flipx(META_LK_WALK2, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000)
			: move_metasprite_ex(META_LK_WALK2, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000);

	} else {
		FLIPPED ? move_metasprite_flipx(META_LK_IDLE, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000)
		: move_metasprite_ex(META_LK_IDLE, 0, 0, 0, pos_x + 0b1000, pos_y + 0b10000);
	}
}
