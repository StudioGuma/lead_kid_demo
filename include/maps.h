#ifndef MAPS_H_
#define MAPS_H_

#define CHECKPOINT_UPDATE \
if (cur_checkpoint == 0) { \
	cur_checkpoint = 1; \
	checkpoint_dmg = damage; \
}

extern FAR_PTR cur_map, right_map, left_map, top_map, bottom_map, cur_song;
extern const uint8_t *cur_map_ptr;
extern const hUGESong_t *cur_song_ptr;

void init_map(const FAR_PTR MAP);
void map_update(void);

#endif
