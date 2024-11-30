#include <gbdk/platform.h>
#include <gbdk/far_ptr.h>
#include <gbdk/metasprites.h>

#include "hUGEDriver.h"
#include "bosses.h"
#include "lead_kid.h"
#include "levels.h"
#include "maps.h"
#include "main.h"
#include "map_data.h"
#include "mus_data.h"
#include "spr_data.h"

FAR_PTR cur_map, right_map, left_map, top_map, bottom_map;
const uint8_t *cur_map_ptr = NULL;
const hUGESong_t *cur_song = NULL;

void init_map(const FAR_PTR MAP)
{
	cur_map = MAP;

	SWITCH_ROM(FAR_SEG(cur_map));
	cur_map_ptr = FAR_OFS(cur_map);
	set_bkg_tiles(0, 0, 0b10100, 0b10010, cur_map_ptr);

	TREASURE_APPEAR_FALSE

	if (MAP == FP(MAP_FACTORY_0)) {
		right_map = FP(MAP_FACTORY_10);
		left_map = FP(MAP_FACTORY_1);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000000;
		init_pos_y = 0b1111000;

	} else if (MAP == FP(MAP_FACTORY_1)) {
		right_map = FP(MAP_FACTORY_0);
		left_map = NULL;
		top_map = NULL;
		bottom_map = NULL;

	} else if (MAP == FP(MAP_FACTORY_10)) {
		right_map = FP(MAP_FACTORY_11);
		left_map = FP(MAP_FACTORY_0);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1001000;

	} else if (MAP == FP(MAP_FACTORY_11)) {
		right_map =FP( MAP_FACTORY_100);
		left_map = FP(MAP_FACTORY_10);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1010000;

	} else if (MAP == FP(MAP_FACTORY_100)) {
		right_map = FP(MAP_FACTORY_101);
		left_map = FP(MAP_FACTORY_11);
		top_map = NULL;
		bottom_map = NULL;

	} else if (MAP == FP(MAP_FACTORY_101)) {
		right_map = NULL;
		left_map = FP(MAP_FACTORY_100);
		top_map = FP(MAP_FACTORY_110);
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1110000;

	} else if (MAP == FP(MAP_FACTORY_110)) {
		right_map = FP(MAP_FACTORY_111);
		left_map = NULL;
		top_map = NULL;
		bottom_map = FP(MAP_FACTORY_101);

		init_pos_x = 0b110000;
		init_pos_y = 0b1111000;

	} else if (MAP == FP(MAP_FACTORY_111)) {
		right_map = FP(MAP_FACTORY_1000);
		left_map = FP(MAP_FACTORY_110);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1101000;

	} else if (MAP == FP(MAP_FACTORY_1000)) {
		CHECKPOINT_UPDATE

		right_map = FP(MAP_FACTORY_1001);
		left_map = FP(MAP_FACTORY_111);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1100000;

	} else if (MAP == FP(MAP_FACTORY_1001)) {
		right_map = FP(MAP_FACTORY_1010);
		left_map = FP(MAP_FACTORY_1000);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1000000;

	} else if (MAP == FP(MAP_FACTORY_1010)) {
		right_map = FP(MAP_FACTORY_1011);
		left_map = FP(MAP_FACTORY_1001);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1000000;
		
	} else if (MAP == FP(MAP_FACTORY_1011)) {
		if (!(P_RANK) && damage == 0)
			TREASURE_APPEAR_TRUE;

		right_map = FP(MAP_FACTORY_1100);
		left_map = FP(MAP_FACTORY_1010);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1101000;

	} else if (MAP == FP(MAP_FACTORY_1100)) {
		right_map = FP(MAP_FACTORY_1110);
		left_map = FP(MAP_FACTORY_1011);
		top_map = FP(MAP_FACTORY_1101);
		bottom_map = NULL;

	} else if (MAP == FP(MAP_FACTORY_1101)) {
		right_map = NULL;
		left_map = NULL;
		top_map = NULL;
		bottom_map = FP(MAP_FACTORY_1100);

		init_pos_x = 0b10000;
		init_pos_y = 0b1110000;

	} else if (MAP == FP(MAP_FACTORY_1110)) {
		right_map = FP(MAP_FACTORY_1111);
		left_map = FP(MAP_FACTORY_1100);
		top_map = NULL;
		bottom_map = NULL;

		init_pos_x = 0b1000;
		init_pos_y = 0b1100000;

	} else if (MAP == FP(MAP_FACTORY_1111)) {
		right_map = NULL;
		left_map = FP(MAP_FACTORY_1110);
		top_map = NULL;
		bottom_map = FP(MAP_FACTORY_10000);

		init_pos_x = 0b1000;
		init_pos_y = 0b1010000;

	} else {
		cur_checkpoint = 0b10;

		right_map = NULL;
		left_map = NULL;
		top_map = FP(MAP_FACTORY_1111);
		bottom_map = NULL;

		init_pos_x = 0b1000000;
		init_pos_y = 0b1110000;

		init_boss();
	}

	if (cur_checkpoint == 0b10) {
		INIT_SONG(MUS_BOSS)
	} else {
		INIT_SONG(MUS_FACTORY)
	}
}

void map_update(void)
{
	if (pos_x > 0b10100000 && pos_x <= 0b11000000) {
		just_paused = 0;
		pos_x -= 0b10100000;
		if (right_map != NULL)
			init_map(right_map);

	} else if (pos_x > 0b11000000) {
		just_paused = 0;
		pos_x += 0b10100000;
		if (left_map != NULL)
			init_map(left_map);

	} else if (pos_y > 0b11000000) {
		just_paused = 0;
		pos_y += 0b10010000;
		if (top_map != NULL)
			init_map(top_map);

	} else if (pos_y > 0b10010000 && pos_y <= 0b11000000) {
		just_paused = 0;
		pos_y -= 0b10010000;
		if (bottom_map != NULL)
			init_map(bottom_map);
	}
}
