#ifndef LEAD_KID_H_
#define LEAD_KID_H_

#define SPD_X 0b11
#define ACL_Y 1
#define JUMP_HEIGHT (int8_t)0b11111011
#define DMG_TIMER_MAX 0b1000000
#define JUMP_BUF_MAX 0b100
#define COYOTE_TIME_MAX 0b10

#define NUM_SOLID_TILES 0b1001
#define SPIKE_UP_TILE 0b10
#define SPIKE_LEFT_TILE 0b11
#define SPIKE_DOWN_TILE 0b100
#define SPIKE_RIGHT_TILE 0b101
#define SPRING_TILE 0b110
#define CONVEYOR_LEFT_TILE 0b111
#define CONVEYOR_RIGHT_TILE 0b1000

#define JUMPING lk_state & 1
#define JUMPING_TRUE lk_state |= 1
#define JUMPING_FALSE lk_state &= ~(1)
#define MOVING lk_state & 0b10
#define MOVING_TRUE lk_state |= 0b10
#define MOVING_FALSE lk_state &= ~(0b10)
#define FLIPPED lk_state & 0b100
#define FLIPPED_TRUE lk_state |= 0b100
#define FLIPPED_FALSE lk_state &= ~(0b100)

#define TREASURE_APPEAR lk_state & 0b1000
#define TREASURE_APPEAR_TRUE lk_state |= 0b1000
#define TREASURE_APPEAR_FALSE \
lk_state &= ~(0b1000); \
hide_sprites_range(0b100, 0b1000);

#define P_RANK lk_state & 0b10000
#define P_RANK_TRUE \
SHOW_WIN; \
lk_state |= 0b10000;
#define P_RANK_FALSE \
HIDE_WIN; \
lk_state &= ~(0b10000);

#define TAKE_DAMAGE \
if (!(INVINCIBLE)) \
	take_damage();

extern uint8_t lk_state, pos_x, pos_y, joy, joy_prev, damage,
jump_buf, coyote_time, init_pos_x, init_pos_y, checkpoint_dmg;
extern int8_t vel_y;

void take_damage(void);
inline uint16_t tile_index(uint8_t x, uint8_t y);
inline uint8_t is_solid(const uint8_t X, const uint8_t Y);
void lead_kid_update(void);

#endif
