#include <gbdk/platform.h>
#include <gbdk/far_ptr.h>
#include <gbdk/metasprites.h>

#include "hUGEDriver.h"
#include "bosses.h"
#include "lead_kid.h"
#include "levels.h"
#include "main.h"
#include "maps.h"
#include "treasure.h"
#include "bkg_data.h"
#include "map_data.h"
#include "spr_data.h"

uint8_t cur_checkpoint;

void restart_from_checkpoint(void)
{
	TREASURE_APPEAR_FALSE
	P_RANK_FALSE

	init_level();
}

void init_level(void)
{
	damage = checkpoint_dmg;

	switch (cur_checkpoint) {
	case 0:
		cur_map = FP(MAP_FACTORY_0);
		init_pos_x = 0b1000000;
		init_pos_y = 0b1111000;
		break;
	default:
		cur_map = FP(MAP_FACTORY_111);
		init_pos_x = 0b1000;
		init_pos_y = 0b1101000;
	}
	
	pos_x = init_pos_x;
	pos_y = init_pos_y;
	vel_y = 0;

	init_state(GAME_STATE);
}
