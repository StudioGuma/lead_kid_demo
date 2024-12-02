#pragma bank 1

#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include <gbdk/far_ptr.h>

#include "sfxplayer.h"
#include "hUGEDriver.h"
#include "bosses.h"
#include "lead_kid.h"
#include "levels.h"
#include "main.h"
#include "maps.h"
#include "sfx_data.h"

BANKREF(BOSSES)

const metasprite_t META_BOSS_FACTORY[] = {
	{.dy = -0b1000, .dx = -0b10000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = S_FLIPX},
	{.dy = 0b1000, .dx = 0, .dtile = 0b11, .props = S_FLIPX},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0, .props = S_FLIPX},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = S_FLIPX},
	METASPR_TERM
};

const metasprite_t META_BOSS_FACTORY_HURT[] = {
	{.dy = -0b1000, .dx = -0b10000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b100, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = S_FLIPX},
	{.dy = 0b1000, .dx = 0, .dtile = 0b100, .props = S_FLIPX},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0, .props = S_FLIPX},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = S_FLIPX},
	METASPR_TERM
};

uint8_t boss_pos_x, boss_pos_y, boss_hp, boss_state, boss_dmg_timer,
boss_init_pos_x, boss_init_pos_y;

void init_boss(void)
{
	if (just_paused == 0) {
		boss_hp = BOSS_HP_MAX;
		BOSS_ATTACK_FALSE;
		boss_pos_x = boss_init_pos_x;
		boss_pos_y = boss_init_pos_y;
		boss_dmg_timer = 0;
		BOSS_DAMAGED_FALSE;
	}
}

void boss_update(void)
{
	if (BOSS_ATTACK) {
		boss_pos_y += 0b100;

		if (is_solid(boss_pos_x, boss_pos_y + 0b1000)) {
			if ((cur_map_ptr[tile_index(boss_pos_x - 0b10000,
			boss_pos_y + 0b1100)] == SPIKE_UP_TILE
			|| cur_map_ptr[tile_index(boss_pos_x + 0b1111,
			boss_pos_y + 0b1100)] == SPIKE_UP_TILE) && !(BOSS_DAMAGED)) {
				HURT_BOSS

			} else {
				PLAY_SOUND(SFX_BONK);
			}
			BOSS_ATTACK_FALSE;
		}

	} else {
		if (boss_pos_y > (BOSS_FACTORY_INIT_Y + (BOSS_HP_MAX << 0b10)
		- (boss_hp << 0b10)))
			boss_pos_y -= 0b10;
		else if (boss_pos_x > pos_x
		&& !is_solid(boss_pos_x - 0b10001, boss_pos_y))
			boss_pos_x--;
		else if (boss_pos_x < pos_x
		&& !is_solid(boss_pos_x + 0b10000, boss_pos_y))
			boss_pos_x++;
		else
			BOSS_ATTACK_TRUE;
	}

	if (abs((int16_t)pos_x - (int16_t)boss_pos_x) < 0b10000
	&& abs((int16_t)pos_y - (int16_t)boss_pos_y) < 0b1000) {
		TAKE_DAMAGE
	}

	if (BOSS_DAMAGED) {
		move_metasprite_ex(META_BOSS_FACTORY_HURT, 0b1000, 0, 0b1000,
		boss_pos_x + 0b1000, boss_pos_y + 0b10000);

		boss_dmg_timer++;
		if (boss_dmg_timer == DMG_TIMER_MAX) {
			boss_dmg_timer = 0;
			BOSS_DAMAGED_FALSE;
		}

	} else {
		move_metasprite_ex(META_BOSS_FACTORY, 0b1000, 0, 0b1000,
		boss_pos_x + 0b1000, boss_pos_y + 0b10000);
	}
}
