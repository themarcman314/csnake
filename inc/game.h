#ifndef GAME
#define GAME
#include "board.h"
#include "score.h"
#include <stdbool.h>

typedef enum {
	STATE_CONFIGURE_SELECTED_WIDTH,
	STATE_CONFIGURE_SELECTED_HEIGHT,
	STATE_CONFIGURE_SELECTED_SNAKE_SPEED,
	STATE_CONFIGURE_SELECTED_WRAPPING,
	STATE_CONFIGURE_SELECTED_PLAY,
	STATE_CONFIGURE_SELECTED_MENU,
	STATE_CONFIGURE_SELECTED_NONE,
} GameConfigureSelectedState;

typedef enum {
	STATE_GAME_WELCOME,
	STATE_GAME_CONFIGURE,
	STATE_GAME_CONFIGURE_INPUT_SETTINGS,
	STATE_GAME_RUN,
	STATE_GAME_END,
	STATE_GAME_HIGH_SCORE,
	STATE_GAME_EXIT
} GameState;

typedef struct Game {
	GameState state;
	bool wrapping;
	unsigned score;
	HighScoreEntry *high_scores;
	int tick_speed;
	Board *b;
	int death_timestamp;
	Input in;
	char player_name[20];
	Sound sound_eat;
	Sound sound_background_music;
	Sound sound_death;
} Game;

Game *game_create();
void game_fsm_run(Game *g);
void game_clean(Game *g);
void UpdateDrawFrame(Game *g);

#endif
