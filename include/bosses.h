#ifndef BOSSES_H_
#define BOSSES_H_

#define BOSS_HP_MAX 0b1000
#define BOSS_ATTACK boss_state & 1
#define BOSS_ATTACK_TRUE boss_state |= 1
#define BOSS_ATTACK_FALSE boss_state &= ~(1)
#define BOSS_DAMAGED boss_state & 0b10
#define BOSS_DAMAGED_TRUE boss_state |= 0b10
#define BOSS_DAMAGED_FALSE boss_state &= ~(0b10)

#define HURT_BOSS \
boss_hp--; \
PLAY_SOUND(SFX_BOSS_DMG); \
BOSS_DAMAGED_TRUE; \
if (boss_hp == 0) \
	init_state(LV_CLEAR_STATE);

#define BOSS_FACTORY_INIT_Y 0b10000

extern const metasprite_t META_BOSS_FACTORY_HURT[];
extern uint8_t boss_pos_x, boss_pos_y, boss_hp, boss_dmg_timer, boss_state,
boss_init_pos_x, boss_init_pos_y;

void init_boss(void);
void boss_update(void);

#endif
