#ifndef INPUT_H
#define INPUT_H
typedef enum { IN_NONE, IN_UP, IN_DOWN, IN_LEFT, IN_RIGHT, IN_PLAY_AGAIN, IN_QUIT } TermInputKey;
void term_enable_raw(void);
TermInputKey term_get_key(void);
#endif
