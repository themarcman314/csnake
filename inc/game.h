#ifndef GAME
#define GAME

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

typedef struct Game Game;

Game *game_create();
void game_fsm_run(Game *g);
void game_clean(Game *g);
void UpdateDrawFrame(Game *g);

#endif
