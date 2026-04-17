#ifndef GAME
#define GAME
void game_fsm_run(void);

typedef enum {
	STATE_CONFIGURE_NAME,
	STATE_CONFIGURE_WIDTH,
	STATE_CONFIGURE_HEIGHT,
	STATE_CONFIGURE_SNAKE_SPEED
} GameConfigureState;
#endif
