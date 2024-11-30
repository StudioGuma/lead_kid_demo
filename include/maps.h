#ifndef MAPS_H_
#define MAPS_H_

#define FP(p) TO_FAR_PTR(p, BANK(p))

#define INIT_SONG(s) \
if (cur_song != &s) { \
	cur_song = &s; \
	hUGE_init(cur_song); \
	reset_sound(); \
}

#define CHECKPOINT_UPDATE \
if (cur_checkpoint == 0) { \
	cur_checkpoint = 1; \
	checkpoint_dmg = damage; \
}

extern FAR_PTR cur_map;
extern const uint8_t *cur_map_ptr;
extern const hUGESong_t *cur_song;

void init_map(const FAR_PTR MAP);
void map_update(void);

#endif
