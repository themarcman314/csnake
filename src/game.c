#include "ansi.h"
#include "board.h"
#include "conf.h"
#include "debug.h"
#include "engine.h"
#include "term.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

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
	InputKey key;
	int score;
	int tick_speed;
	Board *b;
	int death_timestamp;
} Game;

GameState game_welcome(InputKey key);
void game_init(Game *const g);
GameState game_end(Game *const g);
GameState game_run(Game *const g);
GameState game_configure(Game *const g);

void game_init(Game *g) {
	engine_init();
	debug_init();
	g->tick_speed = 1000.0 / TICK_FREQUENCY;
	g->state = STATE_GAME_WELCOME;
}

GameState game_end(Game *const g) {
	display_end(g->b, g->score, g->death_timestamp);
	if (g->key == IN_PLAY_AGAIN || g->key == IN_ENTER) {
		g->score = 0;
		snake_init(g->b);
		food_init(g->b);
		return STATE_GAME_RUN;
	} else if (g->key == IN_CONFIGURE) {
		return STATE_GAME_CONFIGURE;
	}
	return STATE_GAME_END;
}

GameState game_run(Game *g) {
	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= g->tick_speed) {
		last_tick = now;
		snake_head_direction_set(g->b->s);
		snake_update_square_position(g->b->s);
		if (snake_ate_food(g->b->s, g->b->f)) {
			g->score++;
			snake_segment_add(g->b->s);
			food_spawn(g->b);
		}
#ifdef TUI
		if (board_check_all_collisions(g->b)) {
			int x, y;
			snake_get_head_position(g->b->s, &x, &y);
			board_draw_collision(g->b, x, y);
			sleep(1);
			return STATE_GAME_END;
		};
#endif
		board_update(g->b);
#ifdef TUI
		board_draw(g->b, g->score);
#endif
	}
#ifdef GRAPHICAL
	if (board_check_all_collisions(g->b)) {
		g->death_timestamp = millis();
		board_draw(g->b, g->score);
		return STATE_GAME_END;
	};
	board_draw(g->b, g->score);
#endif
	return STATE_GAME_RUN;
}

GameState game_welcome(InputKey key) {
	display_welcome();
	if (key != IN_NONE)
		return STATE_GAME_CONFIGURE;
	return STATE_GAME_WELCOME;
}

GameState game_configure(Game *g) {
	static bool configured_board_height = false;
	static bool configured_board_width = false;
	static float freq = TICK_FREQUENCY;
	static int width = BOARD_WIDTH;
	static int height = BOARD_HEIGHT;
	display_configure(configured_board_width, configured_board_height, freq,
			  width, height);

	float const delta = 0.1; // +- 0.1 Hz
	switch (g->key) {
	case IN_PLAY_AGAIN:
		configured_board_height = false;
		configured_board_width = false;
		g->b = board_create(width, height);
		return STATE_GAME_RUN;

	case IN_ENTER:
		if (configured_board_height && configured_board_width) {
			configured_board_height = false;
			configured_board_width = false;
			g->tick_speed = 1000.0F / freq;
			g->b = board_create(width, height);
			return STATE_GAME_RUN;
		} else if (configured_board_width) {
			configured_board_height = true;
		} else {
			configured_board_width = true;
		}
		break;
	case IN_PLUS:
		if (configured_board_height && configured_board_width) {
			freq += delta;
		} else if (configured_board_width) {
			height++;
		} else {
			width++;
		}
		break;
	case IN_MINUS:
		if (width == 0 || height == 0 || freq == 0)
			break;
		if (configured_board_height && configured_board_width) {
			freq -= delta;
		} else if (configured_board_width) {
			height--;
		} else {
			width--;
		}
		break;
	default:
		break;
	}
	return STATE_GAME_CONFIGURE;
}

void game_fsm_run(void) {
	Game g;
	g.b = NULL;
	game_init(&g);
	while (g.state != STATE_GAME_EXIT) {
		window_get_size();
		window_periodic_start();
		g.key = get_key();
		if (g.key == IN_QUIT)
			g.state = STATE_GAME_EXIT;

		switch (g.state) {
		case STATE_GAME_WELCOME:
			g.state = game_welcome(g.key);
			break;
		case STATE_GAME_RUN:
			snake_head_direction_set_next(g.b->s, g.key);
			g.state = game_run(&g);
			break;
		case STATE_GAME_END:
			g.state = game_end(&g);
			break;
		case STATE_GAME_CONFIGURE:
			g.state = game_configure(&g);
			break;
		case STATE_GAME_EXIT:
			if (g.b) {
				food_destroy(&g.b->f);
				snake_kill(&g.b->s);
				board_destroy(&g.b);
			}

		default:
			g.state = STATE_GAME_EXIT;
			break;
		}
		window_periodic_end();
	}
}

// int const board_border_size = 20;
// int const board_size = 10;

// while (!WindowShouldClose()) {
//	BeginDrawing();
//	ClearBackground(RAYWHITE);
//	int const delta = window_height / board_size;
//	board_draw(board_border_size, board_size, 5, delta, LIGHTGRAY);

//	EndDrawing();
//}
// CloseWindow();
