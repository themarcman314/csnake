#include "board.h"
#include "debug.h"
#include "engine.h"
#include "input.h"
#include "timer.h"
#include <stdio.h>
typedef enum {
	STATE_GAME_WELCOME,
	STATE_GAME_CONFIGURE,
	STATE_GAME_CONFIGURE_INPUT_SETTINGS,
	STATE_GAME_RUN,
	STATE_GAME_END,
	STATE_GAME_EXIT
} GameState;

typedef struct {
	GameState state;
	int score;
	int tick_speed;
	Board *b;
} Game;

void game_init(Game *g);
GameState game_welcome(void);
GameState game_end(Game *g);
GameState game_run(Game *g);
GameState game_configure(Game *g);

void game_init(Game *g) {
	term_enable_raw();
	debug_init();
	g->state = STATE_GAME_WELCOME;
	g->tick_speed = 200;
	g->b = board_create(50, 10);
}

GameState game_end(Game *g) { board_destroy(g->b); return STATE_GAME_EXIT;}

GameState game_run(Game *g) {
	static int last_tick = 0;
	snake_head_set_direction(g->b);
	int now = millis();
	if (now - last_tick >= g->tick_speed) {
		last_tick = now;
		board_update(g->b);
		if (board_check_collisions(g->b)) {
			return STATE_GAME_END;
		};
		board_draw(g->b);
	}
	return STATE_GAME_RUN;
}

GameState game_welcome(void) {
	printf("========= csnake =========\n");
	printf("                          \n");
	printf("   Press a key to start   \n");
	printf("==========================\n");
	while (term_get_key() == IN_NONE);
	return STATE_GAME_CONFIGURE;
}

GameState game_configure(Game *g) {
	return STATE_GAME_RUN;
}

void game_fsm_run(void) {
	Game g;
	game_init(&g);
	while (g.state != STATE_GAME_EXIT) {
		switch (g.state) {
		case STATE_GAME_WELCOME:
			g.state = game_welcome();
			break;
		case STATE_GAME_RUN:
			g.state = game_run(&g);
			break;
		case STATE_GAME_END:
			g.state = game_end(&g);
			break;
		case STATE_GAME_CONFIGURE:
			g.state = game_configure(&g);
			break;

		default:
			g.state = STATE_GAME_EXIT;
			break;
		}
	}
}
