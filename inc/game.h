#ifndef GAME
#define GAME
void game_fsm_run(void);

typedef enum {
	STATE_CONFIGURE_SELECTED_WIDTH,
	STATE_CONFIGURE_SELECTED_HEIGHT,
	STATE_CONFIGURE_SELECTED_SNAKE_SPEED,
	STATE_CONFIGURE_SELECTED_PLAY,
	STATE_CONFIGURE_SELECTED_NONE,
} GameConfigureSelectedState;

typedef struct {
	char name[50];
	int score;
} HighScoreEntry;

#endif
