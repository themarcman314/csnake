#include "game.h"
#include "board.h"
#include "conf.h"
#include "configure.h"
#include "debug.h"
#include "engine.h"
#include "input.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

GameState game_welcome(Input in);
void game_init(Game *g);
GameState game_end(Game *g);
GameState game_run(Game *g);
GameState game_configure(Game *g);
GameState game_high_score(Game *g);
void game_restart(Game *g);

GameConfigureStateTransition conf_transitions[] = {
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_WIDTH, KEY_ENTER,
     STATE_CONFIGURE_WIDTH},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_WRAPPING, KEY_ENTER,
     STATE_CONFIGURE_WRAPPING},
    {STATE_CONFIGURE_WRAPPING, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_HEIGHT, KEY_ENTER,
     STATE_CONFIGURE_HEIGHT},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_SNAKE_SPEED, KEY_ENTER,
     STATE_CONFIGURE_SNAKE_SPEED},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_PLAY, KEY_ENTER,
     STATE_CONFIGURE_APPLY},
    {STATE_CONFIGURE_NAME, STATE_CONFIGURE_SELECTED_NONE, KEY_ENTER,
     STATE_CONFIGURE_MENU},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_PLAY, KEY_B,
     STATE_CONFIGURE_NAME},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_WIDTH, KEY_B,
     STATE_CONFIGURE_NAME},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_HEIGHT, KEY_B,
     STATE_CONFIGURE_NAME},
    {STATE_CONFIGURE_MENU, STATE_CONFIGURE_SELECTED_SNAKE_SPEED, KEY_B,
     STATE_CONFIGURE_NAME},
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

void game_init(Game *g) {
	memset(g, 0, sizeof(Game));
	engine_init();
	debug_init();
	g->tick_speed = 1000.0 / TICK_FREQUENCY;
	g->state = STATE_GAME_WELCOME;
	g->score = 0;
	g->wrapping = true;
	InitAudioDevice();
	g->sound_eat = LoadSound("sounds/munch.mp3");
	g->sound_death = LoadSound("sounds/death.mp3");
	g->sound_background_music = LoadSound("sounds/knox-dior.mp3");
	SetSoundVolume(g->sound_background_music, 0.5);
}

GameState game_end(Game *g) {
	static bool saved = false;
	display_end(g->b, g->score, g->death_timestamp);
	if (!saved) {
		saved = true;
		save_score(g->player_name, g->score);
	}
	if (g->in.in_key == KEY_R || g->in.in_key == KEY_ENTER) {
		game_restart(g);
		return STATE_GAME_RUN;
	} else if (g->in.in_key == KEY_C) {
		return STATE_GAME_CONFIGURE;
	} else if (g->in.in_key == KEY_H) {
		return STATE_GAME_HIGH_SCORE;
	}
	return STATE_GAME_END;
}

GameState game_run(Game *g) {
	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= g->tick_speed) {
		last_tick = now;
		snake_head_direction_set(g->b->s);
		snake_update_square_position(g->b->s, g->b->width, g->b->height,
					     g->wrapping);
		if (snake_ate_food(g->b->s, g->b->f)) {
			PauseSound(g->sound_background_music);
			PlaySound(g->sound_eat);
			ResumeSound(g->sound_background_music);
			g->score++;
			snake_segment_add(g->b->s);
			food_spawn(g->b);
		}
		board_update(g->b);
		if (board_check_all_collisions(g->b)) {
			PauseSound(g->sound_background_music);
			PlaySound(g->sound_death);
			ResumeSound(g->sound_background_music);
			g->death_timestamp = millis();
			board_draw(g->b, g->score, true, true);
			return STATE_GAME_END;
		}
	}
	board_draw(g->b, g->score, false, true);
	return STATE_GAME_RUN;
}

GameState game_welcome(Input in) {
	display_welcome();
	if (in.in_key != KEY_NULL || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		return STATE_GAME_CONFIGURE;
	return STATE_GAME_WELCOME;
}

GameState game_configure(Game *g) {
	static GameConfigureState state_conf;
	static DisplayConfigureInfo info;
	static bool initialized = false;
	if (!initialized) {
		state_conf = STATE_CONFIGURE_NAME;
		info.state_select = STATE_CONFIGURE_SELECTED_WIDTH;
		info.freq = TICK_FREQUENCY;
		info.width = BOARD_WIDTH;
		info.height = BOARD_HEIGHT;
		info.name = g->player_name;
		info.board_wrapping = false;
		initialized = true;
		info.demo = board_create(info.width, info.height);
		init_menu_conf(&info);
	}

	snake_demo(info.demo, info.freq, info.board_wrapping);

	int const num_of_conf_transitions =
	    sizeof(conf_transitions) / sizeof(GameConfigureStateTransition);

	if (conf_logic_funcs[state_conf]) {
		int simulated_in = conf_logic_funcs[state_conf](g, &info);
		if (simulated_in != KEY_NULL)
			g->in.in_key = simulated_in;
	}
	if (conf_display_funcs[state_conf]) {
		conf_display_funcs[state_conf](info);
	}

	for (GameConfigureState i = 0; i < num_of_conf_transitions; i++) {
		GameConfigureStateTransition *t = &conf_transitions[i];
		bool state_match = (state_conf == t->current_state);

		// TODO: remove me this should be redundant
		if (g->in.in_key == KEY_NULL &&
		    IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
		    state_conf == STATE_CONFIGURE_MENU) {
			g->in.in_key = KEY_ENTER;
		}

		bool key_match = (g->in.in_key == t->input_key);
		bool sel_match =
		    (t->selected_item == STATE_CONFIGURE_SELECTED_NONE ||
		     info.state_select == t->selected_item);
		if (state_match && key_match && sel_match) {
			state_conf = t->next_state;
			switch (state_conf) {

			case STATE_CONFIGURE_WIDTH:
				init_width_conf(&info);
				break;
			case STATE_CONFIGURE_HEIGHT:
				init_height_conf(&info);
				break;
			case STATE_CONFIGURE_SNAKE_SPEED:
				init_speed_conf(&info);
				break;
			case STATE_CONFIGURE_WRAPPING:
				init_wrapping_conf(&info);
				break;
			}
			if (!info.demo || info.demo->width != info.width ||
			    info.demo->height != info.height) {
				board_destroy(&info.demo);
				info.demo =
				    board_create(info.width, info.height);
			}
			break;
		}
	}
	if (state_conf == STATE_CONFIGURE_APPLY) {
		initialized = false;
		g->wrapping = info.board_wrapping;
		g->tick_speed = 1000.0F / info.freq;
		g->b = board_create(info.width, info.height);
		return STATE_GAME_RUN;
	}
	return STATE_GAME_CONFIGURE;
}

GameState game_high_score(Game *g) {
	static int num_lines = 0;
	if (g->high_scores == NULL) {
		FILE *f = fopen(HIGH_SCORE_FILE_PATH, "r");
		if (f) {
			num_lines = count_lines_file(f);
			g->high_scores =
			    malloc(sizeof(HighScoreEntry) * num_lines);
			if (g->high_scores) {
				parse_high_score_entries(f, g->high_scores,
							 num_lines);
				sort_highscore_entries(g->high_scores,
						       num_lines);
			}
			fclose(f);
		} else {
			display_high_score(NULL, 0);
		}
	}
	display_high_score(g->high_scores, num_lines);
	if (g->in.in_key == KEY_R) {
		game_restart(g);
		return STATE_GAME_RUN;
	}
	return STATE_GAME_HIGH_SCORE;
}

void game_restart(Game *g) {
	g->score = 0;
	snake_init(g->b);
	food_init(g->b);
}

void game_fsm_run(Game *g) {
	while (!WindowShouldClose()) {
		UpdateDrawFrame(g);
	}
}
Game *game_create() {
	Game *g = malloc(sizeof(Game));
	game_init(g);

	return g;
}

void game_clean(Game *g) {
	if (g->b) {
		food_destroy(&g->b->f);
		snake_kill(&g->b->s);
		board_destroy(&g->b);
	}
	if (g->high_scores != NULL) {
		free(g->high_scores);
		g->high_scores = NULL;
	}
}

void UpdateDrawFrame(Game *g) {
	BeginDrawing();
	get_screen_measurements();
	g->in.in_key = GetKeyPressed();
	if (!IsSoundPlaying(g->sound_background_music))
		PlaySound(g->sound_background_music);

	switch (g->state) {
	case STATE_GAME_WELCOME:
		g->state = game_welcome(g->in);
		break;
	case STATE_GAME_RUN:
		snake_head_direction_set_next(g->b->s, g->in);
		g->state = game_run(g);
		break;
	case STATE_GAME_END:
		g->state = game_end(g);
		break;
	case STATE_GAME_HIGH_SCORE:
		g->state = STATE_GAME_END;
		break;
		g->state = game_high_score(g);
		break;
	case STATE_GAME_CONFIGURE:
		g->state = game_configure(g);
		break;
	case STATE_GAME_EXIT:
		return;

	default:
		g->state = STATE_GAME_EXIT;
		break;
	}
	window_periodic_end();
}
