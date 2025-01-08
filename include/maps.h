#ifndef MAPS_H_
#define MAPS_H_

#define FP(p) TO_FAR_PTR(p, BANK(p))
#define SONG_FP(s) TO_FAR_PTR(&s, BANK(s))

#define INIT_SONG(s) \
if (cur_song != SONG_FP(s)) { \
	saved_bank = CURRENT_BANK; \
	cur_song = SONG_FP(s); \
	SWITCH_ROM(BANK(s)); \
	cur_song_ptr = FAR_OFS(cur_song); \
	hUGE_init(cur_song_ptr); \
	reset_sound(); \
	SWITCH_ROM(saved_bank); \
}

#define CHECKPOINT_UPDATE \
if (cur_checkpoint == 0) { \
	cur_checkpoint = 1; \
	checkpoint_dmg = damage; \
}

extern FAR_PTR cur_map, cur_song;
extern const uint8_t *cur_map_ptr;
extern const hUGESong_t *cur_song_ptr;

void init_map(const FAR_PTR MAP);
void map_update(void);

#endif
