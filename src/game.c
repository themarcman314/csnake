#include "board.h"
#include "debug.h"
#include "engine.h"
#include "input.h"
#include "timer.h"
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
	TermInputKey key;
	int score;
	int tick_speed;
	Board *b;
} Game;

void term_print_size(void);
void term_init();
void term_clear_full(void);
void term_disable_raw(void);
void term_get_offset(const int width, const int height, int *offset_row,
		     int *offset_colums);
void term_board_draw_collision(Board const *const b, int const board_x,
			       int const board_y);

GameState game_welcome(TermInputKey key);
void game_init(Game *const g);
GameState game_end(Game *const g);
GameState game_run(Game *const g);
GameState game_configure(Game *const g);

void game_init(Game *g) {
	term_clear_full();
	term_enable_raw();
	debug_init();
	term_init();
	g->state = STATE_GAME_WELCOME;
	g->tick_speed = 100;
	g->b = board_create(50, 10);
}

GameState game_end(Game *const g) {
	snake_kill(g->b->s);
	food_destroy(g->b->f);
	term_clear_full();
	int offset_rows, offset_colums;
	term_get_offset(36, 9, &offset_rows, &offset_colums);
	printf("\033[%d;%dH", offset_rows, offset_colums);
	printf("============== csnake ==============\n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("            game over :(            \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("                                    \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("            score: %4d            \n", g->score);
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("                                    \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("    Press                    Press  \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("     'r'                      'q'   \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf(" to play again              to quit \n");
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	printf("====================================\n");
	g->score = 0;
	int key;
	key = term_get_key();
	if (key != IN_PLAY_AGAIN && key != IN_QUIT) {
		return STATE_GAME_END;
	}
	if (key == IN_PLAY_AGAIN) {
		snake_create(g->b);
		food_init(g->b);
		return STATE_GAME_RUN;
	}
	return STATE_GAME_EXIT;
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
		if (board_check_all_collisions(g->b)) {
			int x, y;
			snake_get_head_position(g->b->s, &x, &y);
			term_board_draw_collision(g->b, x, y);
			sleep(1);
			return STATE_GAME_END;
		};
		board_update(g->b);
		board_draw(g->b);
	}
	return STATE_GAME_RUN;
}

GameState game_welcome(TermInputKey key) {
	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= 50) {
		last_tick = now;
		term_clear_full();
		int offset_rows, offset_colums;

		term_get_offset(26, 4, &offset_rows, &offset_colums);
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("%s", "========= csnake =========\n");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("%s", "                          \n");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("%s", "   Press a key to start   \n");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("%s", "==========================\n");
	}

	fflush(stdout);
	if (key != IN_NONE)
		return STATE_GAME_CONFIGURE;
	return STATE_GAME_WELCOME;
}

GameState game_configure(Game *g) { return STATE_GAME_RUN; }

void game_fsm_run(void) {
	Game g;
	game_init(&g);
	while (g.state != STATE_GAME_EXIT) {
		term_init();
		g.key = term_get_key();
		if (g.key == IN_QUIT)
			g.state = STATE_GAME_EXIT;
		snake_head_direction_set_next(g.b->s, g.key);

		switch (g.state) {
		case STATE_GAME_WELCOME:
			g.state = game_welcome(g.key);
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
		case STATE_GAME_EXIT:
			food_destroy(g.b->f);
			snake_kill(g.b->s);
			board_destroy(g.b);

		default:
			g.state = STATE_GAME_EXIT;
			break;
		}
	}
}
