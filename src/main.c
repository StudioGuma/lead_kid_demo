/*
Lead Kid Demo
Copyright (C) 2024 StudioGuma

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <gbdk/platform.h>
#include <gbdk/far_ptr.h>
#include <gbdk/metasprites.h>
#include <string.h>

#include "sfxplayer.h"
#include "hUGEDriver.h"
#include "bosses.h"
#include "lead_kid.h"
#include "levels.h"
#include "main.h"
#include "maps.h"
#include "treasure.h"
#include "bkg_data.h"
#include "map_data.h"
#include "mus_data.h"
#include "spr_data.h"

const uint8_t const PAUSE_IDXS[0b100] = {0b11, 0b110, 0b1001, 0b1100};
uint8_t cur_state, prev_state, prev_prev_state, pause_idx, min_pause_idx,
just_paused, options;
uint16_t lv_clear_timer;

inline void write_text(const uint8_t X, const uint8_t Y,
const char *const TEXT)
{
	set_bkg_tiles(X, Y, strlen(TEXT), 1, TEXT);
}

uint8_t log(uint8_t n)
{
	static uint8_t result;
	result = 0;

	while (n > 1) {
		n >>= 1;
		result++;
	}

	return result;
}

inline uint8_t abs(int16_t n)
{
	return (n < 0) ? -n : n;
}

inline void opt_update(const uint8_t OPT)
{
	write_text(0b10000, PAUSE_IDXS[log(OPT)],
	(options & OPT) ? "On " : "Off");
}

void reset_sound(void)
{
	if (SFX_ON)
		rAUDTERM |= 0b10001;
	else
		rAUDTERM &= ~(0b10001);
	if (MUSIC_ON)
		rAUDTERM |= 0b11101110;
	else
		rAUDTERM &= ~(0b11101110);
}

void init_state(const uint8_t STATE)
{
	prev_prev_state = prev_state;
	prev_state = cur_state;

	switch (STATE) {
	case TITLE_STATE:
		HIDE_SPRITES;
		HIDE_WIN;

		SWITCH_ROM(BANK(BKG_TITLE_SCREEN));
		set_bkg_1bpp_data(0, 0b10011, BKG_TITLE_SCREEN);
		SWITCH_ROM(BANK(BKG_NUMBERS));
		set_bkg_1bpp_data(0b110000, 0b1010, BKG_NUMBERS);

		SWITCH_ROM(BANK(MAP_TITLE_SCREEN));
		set_bkg_tiles(0, 0, 0b10100, 0b10010, MAP_TITLE_SCREEN);

		write_text(0b1000, 0b1010, "Demo");
		write_text(1, 0b10000, "2024 StudioGuma");

		SWITCH_ROM(BANK(MUS_TITLE));
		INIT_SONG(MUS_TITLE);

		SHOW_BKG;
		break;

	case GAME_STATE:
		init_map(cur_map);
		if (P_RANK)
			SHOW_WIN;
		SHOW_SPRITES;
		hide_sprites_range(0b100, 0b10000);
		break;

	case OPTIONS_STATE:
		set_bkg_tiles(0, 0, 0b10100, 0b10010, MAP_BLANK);

		write_text(0b110, 0, "Options");

		write_text(0b10, PAUSE_IDXS[0], "Music");
		opt_update(OPT_MUSIC);

		write_text(0b10, PAUSE_IDXS[1], "Sound Effects");
		opt_update(OPT_SFX);

		write_text(0b10, PAUSE_IDXS[0b10], "Half Speed");
		opt_update(OPT_HALF_SPEED);

		write_text(0b10, PAUSE_IDXS[0b11], "Invincible");
		opt_update(OPT_INVINC);

		pause_idx = 0;
		set_bkg_tile_xy(0, PAUSE_IDXS[pause_idx], 0b1111111);
		break;

	case PAUSE_STATE:
		HIDE_WIN;
		HIDE_SPRITES;
		set_bkg_tiles(0, 0, 0b10100, 0b10010, MAP_BLANK);

		write_text(0b1000, 0, "Pause");

		min_pause_idx = 0;
		write_text(0b100, PAUSE_IDXS[0], "Restart from");
		write_text(0b101, PAUSE_IDXS[0] + 1, "Checkpoint");

		write_text(0b100, PAUSE_IDXS[1], "Restart from");
		write_text(0b101, PAUSE_IDXS[1] + 1, "Beginning");

		write_text(0b110, PAUSE_IDXS[0b10], "Options");

		pause_idx = min_pause_idx;
		set_bkg_tile_xy(0b10, PAUSE_IDXS[pause_idx], 0b1111111);
		break;

	case LV_CLEAR_STATE:
		if (P_RANK) {
			INIT_SONG(MUS_LV_CLEAR_P_RANK)
		} else {
			INIT_SONG(MUS_LV_CLEAR)
		}
	}

	cur_state = STATE;
}

void main(void)
{
	SWITCH_ROM(1);

	options = 0b11;

	rAUDENA = AUDENA_ON;
	rAUDTERM |= 0b11111111;
	add_VBL((int_handler)sfx_play_isr);
	add_VBL((int_handler)hUGE_dosound);
	rAUDVOL = 0b1110111;

	// Window layer displays at the bottom right
	WX_REG = 0b10010111;
	WY_REG = 0b10000000;

	SWITCH_ROM(BANK(BKG_UPPERCASE));
	set_bkg_1bpp_data(0b1000001, 0b11010, BKG_UPPERCASE);
	SWITCH_ROM(BANK(BKG_LOWERCASE));
	set_bkg_1bpp_data(0b1100001, 0b11010, BKG_LOWERCASE);
	SWITCH_ROM(BANK(BKG_CHECKPOINT));
	set_bkg_1bpp_data(0b1111011, 0b101, BKG_CHECKPOINT);

	init_state(TITLE_STATE);

	for (;;) {
		joy_prev = joy;
		joy = joypad();

		switch (cur_state) {
		case TITLE_STATE:
			if (joy & (J_A | J_B | J_START | J_SELECT)) {
				HIDE_BKG;
				SWITCH_ROM(BANK(BKG_P_RANK));
				set_bkg_1bpp_data(0b11100, 0b100, BKG_P_RANK);

				set_win_tile_xy(0, 0, 0b11100);
				set_win_tile_xy(0, 1, 0b11101);
				set_win_tile_xy(1, 0, 0b11110);
				set_win_tile_xy(1, 1, 0b11111);

				SWITCH_ROM(BANK(BKG_SPIKES_SPRING));
				set_bkg_1bpp_data(0b10, 0b101, BKG_SPIKES_SPRING);
				SWITCH_ROM(BANK(BKG_CONVEYORS));
				set_bkg_1bpp_data(0b111, 0b10, BKG_CONVEYORS);

				SWITCH_ROM(BANK(BKG_FACTORY));
				set_bkg_1bpp_data(0, 0b10, BKG_FACTORY);
				SWITCH_ROM(BANK(SPR_BOSS_FACTORY));
				set_sprite_1bpp_data(0b1000, 0b101, SPR_BOSS_FACTORY);

				SWITCH_ROM(BANK(BKG_DR_AGON_LOGO));
				set_bkg_1bpp_data(0b100001, 0b1100, BKG_DR_AGON_LOGO);

				SWITCH_ROM(BANK(SPR_LEAD_KID));
				set_sprite_1bpp_data(0, 0b101, SPR_LEAD_KID);
				SWITCH_ROM(BANK(SPR_TREASURE));
				set_sprite_1bpp_data(0b101, 0b11, SPR_TREASURE);

				// Level initialization
				cur_checkpoint = 0;
				checkpoint_dmg = 0;
				cur_map = FP(MAP_FACTORY_0);
				P_RANK_FALSE

				init_pos_x = 0b1000000;
				init_pos_y = 0b1111000;
				treasure_pos_x = 0b101000;
				treasure_pos_y = 0b101000;
				boss_init_pos_x = 0b10001000;
				boss_init_pos_y = BOSS_FACTORY_INIT_Y;
				init_level();
				SHOW_BKG;
			}
			break;

		case GAME_STATE:
			lead_kid_update();
			map_update();
			if (TREASURE_APPEAR)
				treasure_update();
			if (cur_checkpoint == 0b10)
				boss_update();

			if (joy & J_START && !(joy_prev & J_START))
				init_state(PAUSE_STATE);

			if (HALF_SPEED)
				vsync();

			break;

		case OPTIONS_STATE:
			if (joy & (J_B | J_START) && !(joy_prev & (J_B | J_START))) {
				just_paused = 1;
				init_state(prev_prev_state);
			}

			if (joy & J_DOWN && !(joy_prev & J_DOWN)) {
				set_bkg_tile_xy(0, PAUSE_IDXS[pause_idx], 0b100000);
				pause_idx = (pause_idx == 0b11) ? 0 : pause_idx + 1;
				set_bkg_tile_xy(0, PAUSE_IDXS[pause_idx], 0b1111111);
			}

			if (joy & J_UP && !(joy_prev & J_UP)) {
				set_bkg_tile_xy(0, PAUSE_IDXS[pause_idx], 0b100000);
				pause_idx = (pause_idx == 0) ? 0b11 : pause_idx - 1;
				set_bkg_tile_xy(0, PAUSE_IDXS[pause_idx], 0b1111111);
			}

			if (joy & J_A && !(joy_prev & J_A)) {
				options ^= 1 << pause_idx;
				opt_update(1 << pause_idx);

				if (1 << pause_idx == OPT_MUSIC)
					rAUDTERM ^= 0b11101110;

				else if (1 << pause_idx == OPT_SFX)
					rAUDTERM ^= 0b10001;
			}
			break;

		case PAUSE_STATE:
			if (joy & (J_B | J_START) && !(joy_prev & (J_B | J_START))) {
				just_paused = 1;
				init_state(prev_state);
			}

			if (joy & J_DOWN && !(joy_prev & J_DOWN)) {
				set_bkg_tile_xy(0b10, PAUSE_IDXS[pause_idx], 0b100000);
				pause_idx = (pause_idx == 0b10) ? min_pause_idx : pause_idx + 1;
				set_bkg_tile_xy(0b10, PAUSE_IDXS[pause_idx], 0b1111111);
			}

			if (joy & J_UP && !(joy_prev & J_UP)) {
				set_bkg_tile_xy(0b10, PAUSE_IDXS[pause_idx], 0b100000);
				pause_idx = (pause_idx == min_pause_idx) ? 0b10 : pause_idx - 1;
				set_bkg_tile_xy(0b10, PAUSE_IDXS[pause_idx], 0b1111111);
			}

			if (joy & J_A && !(joy_prev && J_A)) {
				switch (pause_idx) {
				case 0:
					if (cur_checkpoint > 1)
						cur_checkpoint = 1;
					restart_from_checkpoint();
					break;
				case 1:
					cur_checkpoint = 0;
					checkpoint_dmg = 0;
					restart_from_checkpoint();
					break;
				case 0b10:
					init_state(OPTIONS_STATE);
				}
			}
			break;

		case LV_CLEAR_STATE:
			lv_clear_timer++;

			if (boss_pos_y < 0b11101111)
				boss_pos_y += 0b100;
			move_metasprite_flipy(META_BOSS_FACTORY_HURT, 0b1000, 0, 0b1000,
			boss_pos_x + 0b1000, boss_pos_y + 0b10000);

			if ((P_RANK ? lv_clear_timer == LV_CLEAR_TIMER_MAX << 1
			: lv_clear_timer == LV_CLEAR_TIMER_MAX)
			|| joy & (J_A | J_B | J_START | J_SELECT)) {
				lv_clear_timer = 0;
				HIDE_BKG;
				init_state(TITLE_STATE);
			}
		}

		vsync();
	}
}
