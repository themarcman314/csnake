#pragma once
#include "board.h"
#include "configure.h"
#include "raylib.h"
#include "score.h"
#include <stdbool.h>

typedef enum {
	STATE_GAME_WELCOME,
	STATE_GAME_CONFIGURE,
	STATE_GAME_CONFIGURE_INPUT_SETTINGS,
	STATE_GAME_RUN,
	STATE_GAME_END,
	STATE_GAME_HIGH_SCORE,
	STATE_GAME_EXIT
} GameState;

struct Game {
	GameState state;
	GameConfigureContext ctx;
	bool wrapping;
	unsigned score;
	HighScoreEntry *high_scores;
	int num_high_scores;
	int tick_speed;
	Board *b;
	int death_timestamp;
	Input in;
	char player_name[20];
	char country_code[2];
	Sound sound_eat;
	Sound sound_background_music;
	Sound sound_death;
	Sound sound_click;
};

Game *game_create();
void game_fsm_run(Game *g);
void game_clean(Game *g);
void UpdateDrawFrame(Game *g);
