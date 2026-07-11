#include "game.h"
#include "board.h"
#include "conf.h"
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

typedef enum {
	STATE_GAME_WELCOME,
	STATE_GAME_CONFIGURE,
	STATE_GAME_CONFIGURE_INPUT_SETTINGS,
	STATE_GAME_RUN,
	STATE_GAME_END,
	STATE_GAME_HIGH_SCORE,
	STATE_GAME_EXIT
} GameState;

typedef enum {
	STATE_CONFIGURE_NAME,
	STATE_CONFIGURE_MENU,
	STATE_CONFIGURE_WIDTH,
	STATE_CONFIGURE_HEIGHT,
	STATE_CONFIGURE_SNAKE_SPEED,
	STATE_CONFIGURE_WRAPPING,
	STATE_CONFIGURE_APPLY,
} GameConfigureState;

typedef struct {
	GameConfigureState current_state;
	GameConfigureSelectedState selected_item;
	KeyboardKey input_key;
	GameConfigureState next_state;
} GameConfigureStateTransition;

struct Game {
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
};

GameState game_welcome(Input in);
void game_init(Game *g);
GameState game_end(Game *g);
GameState game_run(Game *g);
GameState game_configure(Game *g);
GameState game_high_score(Game *g);
void game_restart(Game *g);
void navigate_menu(GameConfigureSelectedState *state, int const direction);
int count_lines_file(FILE *f);
void parse_high_score_entries(FILE *f, HighScoreEntry *h,
			      int const entry_count);
void save_score(char const *name, unsigned const score);
void sort_highscore_entries(HighScoreEntry *h, int const num_entries);

void update_name_conf(Game *g, DisplayConfigureInfo *i);
void update_menu_conf(Game *g, DisplayConfigureInfo *i);
void update_wrapping_conf(Game *g, DisplayConfigureInfo *i);
void update_width_conf(Game *g, DisplayConfigureInfo *i);
void update_height_conf(Game *g, DisplayConfigureInfo *i);
void update_snake_speed_conf(Game *g, DisplayConfigureInfo *i);

void snake_demo(DisplayConfigureInfo info);

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
typedef void (*GameConfigureFunc)(Game *g, DisplayConfigureInfo *i);
static const GameConfigureFunc conf_logic_funcs[] = {
    [STATE_CONFIGURE_NAME] = update_name_conf,
    [STATE_CONFIGURE_MENU] = update_menu_conf,
    [STATE_CONFIGURE_WIDTH] = update_width_conf,
    [STATE_CONFIGURE_HEIGHT] = update_height_conf,
    [STATE_CONFIGURE_WRAPPING] = update_wrapping_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = update_snake_speed_conf,
    //[STATE_CONFIGURE_APPLY] = conf_update_,
};
static const ConfDisplayFunc conf_display_funcs[] = {
    [STATE_CONFIGURE_NAME] = display_name_conf,
    [STATE_CONFIGURE_MENU] = display_menu_conf,
    [STATE_CONFIGURE_WIDTH] = display_width_conf,
    [STATE_CONFIGURE_HEIGHT] = display_height_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = display_snake_speed_conf,
    [STATE_CONFIGURE_WRAPPING] = display_wrapping_conf,
};

void update_wrapping_conf(Game *g, DisplayConfigureInfo *i) {
	g->wrapping = !g->wrapping;
	i->board_wrapping = g->wrapping;
}
void update_menu_conf(Game *g, DisplayConfigureInfo *i) {
	i->element_count = 4;

	int screen_height = GetScreenHeight();
	int screen_width = GetScreenWidth();

	int const border_fraction_screen_width = 15;
	int const border_fraction_screen_height = 15;
	int rectangle_height = screen_height / 20;
	int rectangle_width = (border_fraction_screen_width - 2) *
			      screen_width / border_fraction_screen_width;
	int rectangle_y_base = screen_height / border_fraction_screen_height;
	int rectangle_height_spacing = screen_height / (i->element_count + 1);
	int rectangle_x = screen_width / border_fraction_screen_width;

	Vector2 current_mouse_pos = GetMousePosition();
	bool mouse_moved = (current_mouse_pos.x != i->last_mouse_pos.x ||
			    current_mouse_pos.y != i->last_mouse_pos.y);

	// Save current position for the next frame
	i->last_mouse_pos = current_mouse_pos;

	// assign rectangle boxes
	for (int idx = 0; idx < i->element_count; idx++) {
		i->elements[idx].bounds.x = rectangle_x;
		i->elements[idx].bounds.y =
		    idx * rectangle_height_spacing + rectangle_y_base;
		i->elements[idx].bounds.width = rectangle_width;
		i->elements[idx].bounds.height = rectangle_height;
	}

	int const rectangle_thickness_lines = 2;
	int rectangle_fill_offset = 5;
	char play_button[] = "Play";
	int button_width =
	    MeasureText(play_button, rectangle_height - rectangle_fill_offset);
	// DrawRectangleLinesEx(r, rectangle_thickness_lines, GRAY);
	i->elements[i->element_count].bounds.x = screen_width - rectangle_x -
						 button_width -
						 2 * rectangle_fill_offset;
	i->elements[i->element_count].bounds.y =
	    i->element_count * rectangle_height_spacing + rectangle_y_base;
	i->elements[i->element_count].bounds.width =
	    button_width + rectangle_fill_offset * 2;
	i->elements[i->element_count].bounds.height = rectangle_height;
	// printf("assigned play box\n");

	if (mouse_moved) {
		i->state_select = STATE_CONFIGURE_SELECTED_NONE;
		for (int idx = 0; idx <= i->element_count; idx++) {
			bool is_mouse_over = CheckCollisionPointRec(
			    current_mouse_pos, i->elements[idx].bounds);
			if (is_mouse_over)
				i->state_select = idx;
		}
	}

	else {
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
}

void apply_conf(Game *g, DisplayConfigureInfo *i) {
	g->tick_speed = 1000.0F / i->freq;
	g->b = board_create(i->width, i->height);
}

void update_name_conf(Game *g, DisplayConfigureInfo *i) {
	static int letterCount = 0;
	int char_pressed = GetCharPressed();

	if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
		letterCount--;
		if (letterCount < 0)
			letterCount = 0;
		g->player_name[letterCount] = '\0';
	}
	if (char_pressed > 0) {
		// NOTE: Only allow keys in range [32..125]
		if ((char_pressed >= 32) && (char_pressed <= 125) &&
		    (letterCount < sizeof(g->player_name))) {
			g->player_name[letterCount] = (char)char_pressed;
			g->player_name[letterCount + 1] =
			    '\0'; // Add null terminator at the
				  // end of the string
			if (is_display_name_box_overflown(g->player_name)) {
				if (letterCount < 0)
					letterCount = 0;
				g->player_name[letterCount] = '\0';
				return;
			}
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
		info.board_wrapping = true;
		initialized = true;
		info.demo = board_create(info.width, info.height);
	}

	snake_demo(info);

	int const num_of_conf_transitions =
	    sizeof(conf_transitions) / sizeof(GameConfigureStateTransition);

	for (GameConfigureState i = 0; i < num_of_conf_transitions; i++) {
		GameConfigureStateTransition *t = &conf_transitions[i];
		bool state_match = (state_conf == t->current_state);

		// if left mouse click, count as KEY_ENTER
		if (g->in.in_key == KEY_NULL &&
		    IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			g->in.in_key = KEY_ENTER;

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
		initialized = false;
		g->tick_speed = 1000.0F / info.freq;
		g->b = board_create(info.width, info.height);
		return STATE_GAME_RUN;
	}
	return STATE_GAME_CONFIGURE;
}

void snake_demo(DisplayConfigureInfo info) {
	static int last_tick = 0;
	static int last_tick_input = 0;
	static Input in = {KEY_RIGHT};
	int now = millis();
	if (now - last_tick >= 1000.0F / info.freq) {
		last_tick = now;
		if (now - last_tick_input >= 200.0F) {
			last_tick_input = now;
			in.in_key =
			    (rand() % (4 + 1)) + KEY_RIGHT; // random input
		}

		snake_head_direction_set_next(info.demo->s, in);
		snake_head_direction_set(info.demo->s);
		if (info.board_wrapping == false) {
			while (snake_check_board_imminent_collision(
			    info.demo)) { // make sure we don't collide
					  // with edge
				snake_right_direction_to_current(info.demo->s);
			}
		}
		snake_update_square_position(info.demo->s, info.demo->width,
					     info.demo->height,
					     info.board_wrapping);
		if (snake_ate_food(info.demo->s, info.demo->f)) {
			snake_segment_add(info.demo->s);
			food_spawn(info.demo);
		}
		if (board_check_all_collisions(info.demo)) {
			// restart demo
			snake_init(info.demo);
			food_init(info.demo);
		}
		board_update(info.demo);
	}
}

void navigate_menu(GameConfigureSelectedState *state, int const direction) {
	*state = (*state + direction + 5) % 5;
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

void sort_highscore_entries(HighScoreEntry *h, int const num_entries) {
	HighScoreEntry temp;
	bool sorted = true;
	do {
		sorted = true;
		for (int i = 0; i < num_entries - 1; i++) {
			HighScoreEntry *current = h + i;
			HighScoreEntry *next = current + 1;
			if (current->score < next->score) {
				sorted = false;
				memcpy(&temp, current, sizeof(HighScoreEntry));
				memcpy(current, next, sizeof(HighScoreEntry));
				memcpy(next, &temp, sizeof(HighScoreEntry));
			}
		}
	} while (!sorted);
}

// FIXME: implement version with sockets for web version
//  highscores should be saved on vps not on client machine lol
void save_score(char const *name, unsigned const score) {
	FILE *f = fopen(HIGH_SCORE_FILE_PATH, "a");
	if (f) {
		// fseek(f, 0, SEEK_END);
		if (strlen(name) > 0)
			fprintf(f, "%s,%d\n", name, score);
		fclose(f);
	}
}

void game_restart(Game *g) {
	g->score = 0;
	snake_init(g->b);
	food_init(g->b);
}

void parse_high_score_entries(FILE *f, HighScoreEntry *h,
			      int const entry_count) {
	for (int i = 0; i < entry_count; i++)
		memset(h[i].name, 0, sizeof(h->name));
	int const line_size = 100;
	char line[line_size];
	for (int i = 0; i < entry_count; i++) {
		fgets(line, line_size, f);
		char *end_line = line;
		while (*end_line != ',')
			end_line++;
		char name[20] = "";
		memcpy(h[i].name, line, end_line - line);
		h[i].score = atoi((char *)(end_line + 1));
	}
}

int count_lines_file(FILE *f) {
	fseek(f, 0, SEEK_SET); // move to start of file
	char c;
	int num_lines = 0;
	while ((c = fgetc(f)) != EOF) {
		if (c == '\n')
			num_lines++;
	}
	fseek(f, 0, SEEK_SET); // move to start of file
	return num_lines;
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
