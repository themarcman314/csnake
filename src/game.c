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

typedef enum {
	STATE_CONFIGURE_NAME,
	STATE_CONFIGURE_MENU,
	STATE_CONFIGURE_WIDTH,
	STATE_CONFIGURE_HEIGHT,
	STATE_CONFIGURE_SNAKE_SPEED,
	STATE_CONFIGURE_APPLY,
} GameConfigureState;

typedef struct {
	GameConfigureState current_state;
	GameConfigureSelectedState selected_item;
	KeyboardKey input_key;
	GameConfigureState next_state;
} GameConfigureStateTransition;

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

void update_name_conf(Game *g, DisplayConfigureInfo *i);
void update_menu_conf(Game *g, DisplayConfigureInfo *i);
void update_width_conf(Game *g, DisplayConfigureInfo *i);
void update_height_conf(Game *g, DisplayConfigureInfo *i);
void update_snake_speed_conf(Game *g, DisplayConfigureInfo *i);

GameConfigureStateTransition conf_transitions[] = {
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_WIDTH, KEY_ENTER,
     STATE_CONFIGURE_WIDTH},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_HEIGHT, KEY_ENTER,
     STATE_CONFIGURE_HEIGHT},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_SNAKE_SPEED, KEY_ENTER,
     STATE_CONFIGURE_SNAKE_SPEED},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_PLAY, KEY_ENTER,
     STATE_CONFIGURE_APPLY},
    {STATE_CONFIGURE_NAME, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_WIDTH, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_HEIGHT, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_SNAKE_SPEED, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_WIDTH, STATE_CONFIGURE_SELECTED_NONE, KEY_B,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_HEIGHT, STATE_CONFIGURE_SELECTED_NONE, KEY_B,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_SNAKE_SPEED, STATE_CONFIGURE_SELECTED_NONE, KEY_B,
     STATE_CONFIGURE_MENU},
};
typedef void (*GameConfigureFunc)(Game *g, DisplayConfigureInfo *i);
static const GameConfigureFunc conf_logic_funcs[] = {
    [STATE_CONFIGURE_NAME] = update_name_conf,
    [STATE_CONFIGURE_MENU] = update_menu_conf,
    [STATE_CONFIGURE_WIDTH] = update_width_conf,
    [STATE_CONFIGURE_HEIGHT] = update_height_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = update_snake_speed_conf,
    //[STATE_CONFIGURE_APPLY] = conf_update_,
};
static const ConfDisplayFunc conf_display_funcs[] = {
    [STATE_CONFIGURE_NAME] = display_name_conf,
    [STATE_CONFIGURE_MENU] = display_menu_conf,
    [STATE_CONFIGURE_WIDTH] = display_width_conf,
    [STATE_CONFIGURE_HEIGHT] = display_height_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = display_snake_speed_conf,
};

void update_menu_conf(Game *g, DisplayConfigureInfo *i) {
	switch (g->in.in_key) {
	case KEY_J:
	case KEY_DOWN:
		navigate_menu(&i->state_select, +1);
		break;
	case KEY_K:
	case KEY_UP:
		navigate_menu(&i->state_select, -1);
		break;
	}
}

void apply_conf(Game *g, DisplayConfigureInfo *i) {
	g->tick_speed = 1000.0F / i->freq;
	g->b = board_create(i->width, i->height);
}

void update_name_conf(Game *g, DisplayConfigureInfo *i) {
	static int letterCount = 0;
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
			g->player_name[letterCount] = (char)char_pressed;
			g->player_name[letterCount + 1] =
			    '\0'; // Add null terminator at the
				  // end of the string
			letterCount++;
		}
	}
}
void update_width_conf(Game *g, DisplayConfigureInfo *i) {
	if (g->in.in_key == KEY_EQUAL || IsKeyPressedRepeat(KEY_EQUAL)) {
		i->width++;
	} else if (g->in.in_key == KEY_MINUS || IsKeyPressedRepeat(KEY_MINUS)) {
		if (i->width == 2)
			return;
		i->width--;
	}
}

void update_height_conf(Game *g, DisplayConfigureInfo *i) {
	if (g->in.in_key == KEY_EQUAL || IsKeyPressedRepeat(KEY_EQUAL)) {
		i->height++;
	} else if (g->in.in_key == KEY_MINUS || IsKeyPressedRepeat(KEY_MINUS)) {
		if (i->height == 2)
			return;
		i->height--;
	}
	i->demo = board_create(i->width, i->height);
}

void update_snake_speed_conf(Game *g, DisplayConfigureInfo *i) {
	float const delta = 0.1; // +- 0.1 Hz
	if (g->in.in_key == KEY_EQUAL || IsKeyPressedRepeat(KEY_EQUAL)) {
		i->freq += delta;
	} else if (g->in.in_key == KEY_MINUS || IsKeyPressedRepeat(KEY_MINUS)) {
		if (i->freq < 0.6f)
			return;
		i->freq -= delta;
	}
}

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
	static GameConfigureState state_conf;
	static DisplayConfigureInfo info;
	static bool initialized = false;
	if (!initialized) {
		state_conf = STATE_CONFIGURE_NAME;
		info.demo = NULL;
		info.state_select = STATE_CONFIGURE_SELECTED_WIDTH;
		info.freq = TICK_FREQUENCY;
		info.width = BOARD_WIDTH;
		info.height = BOARD_HEIGHT;
		info.name = g->player_name;
		initialized = true;
	}

	int const num_of_conf_transitions =
	    sizeof(conf_transitions) / sizeof(GameConfigureStateTransition);

	for (GameConfigureState i = 0; i < num_of_conf_transitions; i++) {
		GameConfigureStateTransition *t = &conf_transitions[i];
		bool state_match = (state_conf == t->current_state);
		bool key_match = (g->in.in_key == t->input_key);
		bool sel_match =
		    (t->selected_item == STATE_CONFIGURE_SELECTED_NONE ||
		     info.state_select == t->selected_item);
		if (state_match && key_match && sel_match) {
			state_conf = t->next_state;
			break;
		}
	}
	if (conf_logic_funcs[state_conf]) {
		conf_logic_funcs[state_conf](g, &info);
	}
	if (conf_display_funcs[state_conf]) {
		conf_display_funcs[state_conf](info);
	}
	if (state_conf == STATE_CONFIGURE_APPLY) {
		g->tick_speed = 1000.0F / info.freq;
		g->b = board_create(info.width, info.height);
		return STATE_GAME_RUN;
	}
	return STATE_GAME_CONFIGURE;
	//	info.state_conf = STATE_CONFIGURE_NAME;
	//	food_destroy(&info.demo->f);
	//	snake_kill(&info.demo->s);
	//	board_destroy(&info.demo);
	//	break;
}

void navigate_menu(GameConfigureSelectedState *state, int const direction) {
	*state = (*state + direction + 4) % 4;
}

void game_fsm_run(void) {
	Game g;
	g.b = NULL;
	game_init(&g);
	while (g.state != STATE_GAME_EXIT) {
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
