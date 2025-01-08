#ifndef MAIN_H_
#define MAIN_H_

#define TITLE_STATE 0
#define GAME_STATE 1
#define OPTIONS_STATE 0b10
#define PAUSE_STATE 0b11
#define LV_SEL_STATE 0b100
#define LV_CLEAR_STATE 0b101

#define LV_CLEAR_TIMER_MAX 0b11000000

#define OPT_MUSIC 1
#define OPT_SFX 0b10
#define OPT_HALF_SPEED 0b100
#define OPT_INVINC 0b1000

#define MUSIC_ON options & OPT_MUSIC
#define SFX_ON options & OPT_SFX
#define HALF_SPEED options & OPT_HALF_SPEED
#define INVINCIBLE options & OPT_INVINC

#define PLAY_SOUND(s) sfx_set_sample(BANK(s), s)

extern uint8_t cur_state, just_paused, options, saved_bank;

inline void write_text(const uint8_t X, const uint8_t Y,
const char *const TEXT);
inline uint8_t abs(int16_t n);
void reset_sound(void);
void init_state(const uint8_t STATE);

#endif
