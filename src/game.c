#include "game.h"
#include "board.h"
#include "conf.h"
#include "debug.h"
#include "engine.h"
#include "input.h"
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
	int score;
	int tick_speed;
	Board *b;
	int death_timestamp;
	Input in;
	char player_name[20];
} Game;

GameState game_welcome(Input in);
void game_init(Game *const g);
GameState game_end(Game *const g);
GameState game_run(Game *const g);
GameState game_configure(Game *const g);
void navigate_menu(GameConfigureSelectedState *state, int const direction);

void conf_decrement(GameConfigureState state, float const delta, int *height,
		    int *width, float *freq);
void conf_increment(GameConfigureState state, float const delta, int *height,
		    int *width, float *freq);

void game_init(Game *g) {
	engine_init();
	debug_init();
	g->tick_speed = 1000.0 / TICK_FREQUENCY;
	g->state = STATE_GAME_WELCOME;
	g->score = 0;
}

GameState game_end(Game *const g) {
	display_end(g->b, g->score, g->death_timestamp);
	if (g->in.in_key == KEY_R || g->in.in_key == KEY_ENTER) {
		g->score = 0;
		snake_init(g->b);
		food_init(g->b);
		return STATE_GAME_RUN;
	} else if (g->in.in_key == KEY_C) {
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
		board_update(g->b);
	}
	if (board_check_all_collisions(g->b)) {
		g->death_timestamp = millis();
		return STATE_GAME_END;
	};
	// should be called every itteration since
	board_draw(g->b, g->score, true);
	return STATE_GAME_RUN;
}

GameState game_welcome(Input in) {
	display_welcome();
	if (in.in_key != KEY_NULL)
		return STATE_GAME_CONFIGURE;
	return STATE_GAME_WELCOME;
}

GameState game_configure(Game *g) {
	static int letterCount = 0;
	static DisplayConfigureInfo info;
	static bool initialized = false;
	if (!initialized) {
		info.demo = NULL;
		info.state_conf = STATE_CONFIGURE_NAME;
		info.state_select = STATE_SELECTED_WIDTH;
		info.freq = TICK_FREQUENCY;
		info.width = BOARD_WIDTH;
		info.height = BOARD_HEIGHT;
		info.name = g->player_name;
		initialized = true;
	}

	display_configure(info);

	float const delta = 0.1; // +- 0.1 Hz

	if (IsKeyPressedRepeat(KEY_EQUAL)) {
		conf_increment(info.state_conf, delta, &info.height,
			       &info.width, &info.freq);
	} else if (IsKeyPressedRepeat(KEY_MINUS)) {
		conf_decrement(info.state_conf, delta, &info.height,
			       &info.width, &info.freq);
	}

	switch (info.state_conf) {
	case STATE_CONFIGURE_MENU:
		switch (g->in.in_key) {
		case KEY_ENTER:
			info.state_conf = STATE_CONFIGURE_WIDTH;
			break;
		case KEY_J:
		case KEY_DOWN:
			navigate_menu(&info.state_select, +1);
			break;
		case KEY_K:
		case KEY_UP:
			navigate_menu(&info.state_select, -1);
			break;
		}
		break;

	case STATE_CONFIGURE_NAME:
		switch (g->in.in_key) {
		case KEY_ENTER:
			info.state_conf = STATE_CONFIGURE_MENU;
			break;
		}
		int char_pressed = GetCharPressed();

		if (IsKeyPressed(KEY_BACKSPACE)) {
			letterCount--;
			if (letterCount < 0)
				letterCount = 0;
			g->player_name[letterCount] = '\0';
		}
		if (char_pressed > 0) {
			// NOTE: Only allow keys in range [32..125]
			if ((char_pressed >= 32) && (char_pressed <= 125) &&
			    (letterCount < 20)) {
				g->player_name[letterCount] =
				    (char)char_pressed;
				g->player_name[letterCount + 1] =
				    '\0'; // Add null terminator at the
					  // end of the string
				letterCount++;
			}
		}
		break;
	case STATE_CONFIGURE_WIDTH:
		switch (g->in.in_key) {
		case KEY_ENTER:
			info.state_conf = STATE_CONFIGURE_HEIGHT;
			break;
		case KEY_R:
			info.state_conf = STATE_CONFIGURE_NAME;
			g->b = board_create(info.width, info.height);
			return STATE_GAME_RUN;

		case KEY_EQUAL:
			conf_increment(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
			break;
		case KEY_MINUS:
			conf_decrement(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
			break;
		}
		break;
	case STATE_CONFIGURE_HEIGHT:
		switch (g->in.in_key) {
		case KEY_ENTER:
			info.state_conf = STATE_CONFIGURE_SNAKE_SPEED;
			break;
		case KEY_R:
			info.state_conf = STATE_CONFIGURE_NAME;
			g->b = board_create(info.width, info.height);
			return STATE_GAME_RUN;
		case KEY_EQUAL:
			conf_increment(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
			break;
		case KEY_MINUS:
			conf_decrement(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
			break;
		}
		info.demo = board_create(info.width, info.height);
		break;
	case STATE_CONFIGURE_SNAKE_SPEED:
		switch (g->in.in_key) {
		case KEY_ENTER:
			info.state_conf = STATE_CONFIGURE_NAME;
			food_destroy(&info.demo->f);
			snake_kill(&info.demo->s);
			board_destroy(&info.demo);
			g->tick_speed = 1000.0F / info.freq;
			g->b = board_create(info.width, info.height);
			return STATE_GAME_RUN;
			break;
		case KEY_EQUAL:
			conf_increment(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
			break;
		case KEY_MINUS:
			conf_decrement(info.state_conf, delta, &info.height,
				       &info.width, &info.freq);
		}
		break;
	}
	return STATE_GAME_CONFIGURE;
}

void navigate_menu(GameConfigureSelectedState *state, int const direction) {
	*state =
	    (*state + direction + STATE_SELECTED_COUNT) % STATE_SELECTED_COUNT;
}

void game_fsm_run(void) {
	Game g;
	g.b = NULL;
	game_init(&g);
	while (g.state != STATE_GAME_EXIT) {
		window_get_size();
		window_periodic_start();
		g.in.in_key = GetKeyPressed();
		if (WindowShouldClose())
			g.state = STATE_GAME_EXIT;

		switch (g.state) {
		case STATE_GAME_WELCOME:
			g.state = game_welcome(g.in);
			break;
		case STATE_GAME_RUN:
			snake_head_direction_set_next(g.b->s, g.in);
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
			return;

		default:
			g.state = STATE_GAME_EXIT;
			break;
		}
		window_periodic_end();
	}
}

void conf_decrement(GameConfigureState state, float const delta, int *height,
		    int *width, float *freq) {

	if (state == STATE_CONFIGURE_SNAKE_SPEED) {
		if (*freq < 0.6)
			return;
		*freq -= delta;
	} else if (state == STATE_CONFIGURE_HEIGHT) {
		if (*height == 2)
			return;
		(*height)--;
	} else {
		if (*width == 2)
			return;
		(*width)--;
	}
}

void conf_increment(GameConfigureState state, float const delta, int *height,
		    int *width, float *freq) {

	if (state == STATE_CONFIGURE_SNAKE_SPEED) {
		*freq += delta;
	} else if (state == STATE_CONFIGURE_HEIGHT) {
		(*height)++;
	} else {
		(*width)++;
	}
}
