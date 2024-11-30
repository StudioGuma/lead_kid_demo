#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "sfxplayer.h"
#include "lead_kid.h"
#include "main.h"
#include "sfx_data.h"

const metasprite_t META_TREASURE[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0, .props = S_FLIPX},
	{.dy = 0b1000, .dx = 0, .dtile = 0b10, .props = 0},
	METASPR_TERM
};

uint8_t treasure_pos_x, treasure_pos_y;

void treasure_update(void)
{
	move_metasprite_ex(META_TREASURE, 0b101, 0, 0b100, treasure_pos_x + 0b1000,
	treasure_pos_y + 0b10000);

	if (abs((int16_t)pos_x - (int16_t)treasure_pos_x) < 0b1000 &&
	abs((int16_t)pos_y - (int16_t)treasure_pos_y) < 0b1000) {
		PLAY_SOUND(SFX_TREASURE);
		TREASURE_APPEAR_FALSE
		P_RANK_TRUE
	}
}
