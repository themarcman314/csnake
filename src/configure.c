#include "configure.h"
#include "engine.h"
#include "game.h"
#include "raylib.h"
#include <stdbool.h>
#include <string.h>

// void navigate_menu(UIElement *el, GameConfigureSelectedState *state,
//		   int const direction);

void navigate_menu(DisplayConfigureInfo *info, int const direction);

void navigate_menu(DisplayConfigureInfo *info, int const direction) {
	int num_buttons = info->menu_element_count + 1;
	for (int i = 0; i < info->menu_element_count + 1; i++)
		info->menu_elements[i].is_hovered = false;
	info->state_select =
	    (info->state_select + direction + num_buttons) % num_buttons;
	info->menu_elements[info->state_select].is_hovered = true;
	//*state = (*state + direction + 5) % 5;
}

int update_wrapping_conf(Game *g, DisplayConfigureInfo *info) {
	Vector2 mouse_pos = GetMousePosition();
	for (int i = 0; i < info->sub_element_count; i++) {
		// 1. Update hover status
		info->sub_elements[i].is_hovered = CheckCollisionPointRec(
		    mouse_pos, info->sub_elements[i].bounds);

		// 2. Handle click logic
		if (info->sub_elements[i].is_hovered &&
		    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(g->sound_click);
			if (info->sub_elements[i].id == BTN_TOGGLE) {
				info->board_wrapping = !info->board_wrapping;
			} else if (info->sub_elements[i].id == BTN_ACCEPT) {
				return KEY_ENTER;
			}
		}
	}
	return KEY_NULL;
}

int update_menu_conf(Game *g, DisplayConfigureInfo *i) {

	int screen_height = GetScreenHeight();
	int screen_width = GetScreenWidth();

	Vector2 current_mouse_pos = GetMousePosition();
	bool mouse_moved = (current_mouse_pos.x != i->last_mouse_pos.x ||
			    current_mouse_pos.y != i->last_mouse_pos.y);

	// Save current position for the next frame
	i->last_mouse_pos = current_mouse_pos;

	if (mouse_moved) {
		i->state_select = STATE_CONFIGURE_SELECTED_NONE;
		for (int idx = 0; idx <= i->menu_element_count; idx++) {
			bool is_mouse_over = CheckCollisionPointRec(
			    current_mouse_pos, i->menu_elements[idx].bounds);
			if (is_mouse_over) {
				i->menu_elements[idx].is_hovered = true;
				i->state_select = idx;
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					PlaySound(g->sound_click);
					return KEY_ENTER;
				}
			} else {
				i->menu_elements[idx].is_hovered = false;
			}
		}
	}

	else {
		switch (g->in.in_key) {
		case KEY_J:
		case KEY_DOWN:
			navigate_menu(i, +1);
			break;
		case KEY_K:
		case KEY_UP:
			navigate_menu(i, -1);
			break;
		}
	}
	return KEY_NULL;
}

void apply_conf(Game *g, DisplayConfigureInfo *i) {
	g->tick_speed = 1000.0F / i->freq;
	g->b = board_create(i->width, i->height);
}

int update_name_conf(Game *g, DisplayConfigureInfo *i) {
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
				return KEY_NULL;
			}
			letterCount++;
		}
	}
	return KEY_NULL;
}

int update_width_conf(Game *g, DisplayConfigureInfo *info) {
	Vector2 mouse_pos = GetMousePosition();
	for (int i = 0; i < info->sub_element_count; i++) {
		// 1. Update hover status
		info->sub_elements[i].is_hovered = CheckCollisionPointRec(
		    mouse_pos, info->sub_elements[i].bounds);

		// 2. Handle click logic
		if (info->sub_elements[i].is_hovered &&
		    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(g->sound_click);
			if (info->sub_elements[i].id == BTN_DECREASE) {
				info->width--;
			} else if (info->sub_elements[i].id == BTN_INCREASE) {
				info->width++;
			} else if (info->sub_elements[i].id == BTN_ACCEPT) {
				return KEY_ENTER;
			}
		}
	}
	if (g->in.in_key == KEY_EQUAL || IsKeyPressedRepeat(KEY_EQUAL)) {
		info->width++;
	} else if (g->in.in_key == KEY_MINUS || IsKeyPressedRepeat(KEY_MINUS)) {
		if (info->width == 2)
			return KEY_NULL;
		info->width--;
	}
	return KEY_NULL;
}

int update_height_conf(Game *g, DisplayConfigureInfo *info) {
	int screen_width = GetScreenWidth();
	int screen_height = GetScreenHeight();
	int font_size_big = 35;

	int btn_width = 50, btn_height = 50;
	int inner_spacing = 50;

	Vector2 mouse_pos = GetMousePosition();
	for (int i = 0; i < info->sub_element_count; i++) {
		// 1. Update hover status
		info->sub_elements[i].is_hovered = CheckCollisionPointRec(
		    mouse_pos, info->sub_elements[i].bounds);

		// 2. Handle click logic
		if (info->sub_elements[i].is_hovered &&
		    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(g->sound_click);
			if (info->sub_elements[i].id == BTN_DECREASE) {
				info->height--;
			} else if (info->sub_elements[i].id == BTN_INCREASE) {
				info->height++;
			} else if (info->sub_elements[i].id == BTN_ACCEPT) {
				return KEY_ENTER;
			}
		}
	}

	if (g->in.in_key == KEY_EQUAL || IsKeyPressedRepeat(KEY_EQUAL)) {
		info->height++;
	} else if (g->in.in_key == KEY_MINUS || IsKeyPressedRepeat(KEY_MINUS)) {
		if (info->height == 2)
			return KEY_NULL;
		info->height--;
	}
	return KEY_NULL;
}

int update_snake_speed_conf(Game *g, DisplayConfigureInfo *info) {
	float const delta = 0.1; // +- 0.1 Hz
	float const fast_delta = 0.5;

	Vector2 mouse_pos = GetMousePosition();
	for (int i = 0; i < info->sub_element_count; i++) {
		// 1. Update hover status
		info->sub_elements[i].is_hovered = CheckCollisionPointRec(
		    mouse_pos, info->sub_elements[i].bounds);

		// 2. Handle click logic
		if (info->sub_elements[i].is_hovered &&
		    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(g->sound_click);
			if (info->sub_elements[i].id == BTN_DECREASE) {
				if (!(info->freq < 0.6f))
					info->freq -= delta;
			} else if (info->sub_elements[i].id == BTN_INCREASE) {
				info->freq += delta;
			} else if (info->sub_elements[i].id == BTN_ACCEPT) {
				return KEY_ENTER;
			}
		}
	}

	if (g->in.in_key == KEY_EQUAL) {
		info->freq += delta;
	} else if (IsKeyPressedRepeat(KEY_EQUAL)) {
		info->freq += fast_delta;
	} else if (g->in.in_key == KEY_MINUS) {
		if (info->freq < 0.6f)
			return KEY_NULL;
		info->freq -= delta;
	} else if (IsKeyPressedRepeat(KEY_MINUS)) {
		if (info->freq < 0.6f)
			return KEY_NULL;
		info->freq -= fast_delta;
	}
	return KEY_NULL;
}

int update_wrap_conf(Game *g, DisplayConfigureInfo *info) {}

void init_width_conf(DisplayConfigureInfo *i) {
	i->sub_element_count = 3; // We are using 3 buttons

	// Button 0: Decrease Width
	i->sub_elements[0].bounds = (Rectangle){100, 200, 50, 50};
	memset(i->sub_elements[0].text, 0, sizeof(i->menu_elements[0].text));
	strcpy(i->sub_elements[0].text, "-");
	i->sub_elements[0].id = BTN_DECREASE;
	i->sub_elements[0].outline_thickness = 2.f;

	// Button 1: Increase Width
	i->sub_elements[1].bounds = (Rectangle){200, 200, 50, 50};
	memset(i->sub_elements[1].text, 0, sizeof(i->menu_elements[1].text));
	strcpy(i->sub_elements[1].text, "+");
	i->sub_elements[1].id = BTN_INCREASE;
	i->sub_elements[1].outline_thickness = 2.f;

	// Button 2: Accept
	i->sub_elements[2].bounds = (Rectangle){100, 300, 150, 50};
	memset(i->sub_elements[2].text, 0, sizeof(i->menu_elements[2].text));
	strcpy(i->sub_elements[2].text, "OK");
	i->sub_elements[2].id = BTN_ACCEPT;
	i->sub_elements[2].outline_thickness = 2.f;
}

void init_height_conf(DisplayConfigureInfo *i) {
	i->sub_element_count = 3; // We are using 3 buttons

	// Button 0: Decrease Width
	i->sub_elements[0].bounds = (Rectangle){100, 200, 50, 50};
	strcpy(i->sub_elements[0].text, "-");
	i->sub_elements[0].id = BTN_DECREASE;
	i->sub_elements[0].outline_thickness = 2.f;

	// Button 1: Increase Width
	i->sub_elements[1].bounds = (Rectangle){200, 200, 50, 50};
	strcpy(i->sub_elements[1].text, "+");
	i->sub_elements[1].id = BTN_INCREASE;
	i->sub_elements[1].outline_thickness = 2.f;

	// Button 2: Accept
	i->sub_elements[2].bounds = (Rectangle){100, 300, 150, 50};
	strcpy(i->sub_elements[2].text, "OK");
	i->sub_elements[2].id = BTN_ACCEPT;
	i->sub_elements[2].outline_thickness = 2.f;
}

void init_speed_conf(DisplayConfigureInfo *i) {
	i->sub_element_count = 3; // We are using 3 buttons

	i->sub_elements[0].bounds = (Rectangle){100, 200, 50, 50};
	strcpy(i->sub_elements[0].text, "-");
	i->sub_elements[0].id = BTN_DECREASE;
	i->sub_elements[0].outline_thickness = 2.f;

	i->sub_elements[1].bounds = (Rectangle){200, 200, 50, 50};
	strcpy(i->sub_elements[1].text, "+");
	i->sub_elements[1].id = BTN_INCREASE;
	i->sub_elements[1].outline_thickness = 2.f;

	i->sub_elements[2].bounds = (Rectangle){100, 300, 150, 50};
	strcpy(i->sub_elements[2].text, "OK");
	i->sub_elements[2].id = BTN_ACCEPT;
	i->sub_elements[2].outline_thickness = 2.f;
}

void init_wrapping_conf(DisplayConfigureInfo *i) {
	i->sub_element_count = 2;

	i->sub_elements[0].bounds = (Rectangle){100, 200, 150, 50};
	sprintf(i->sub_elements[0].text,
		i->board_wrapping ? "enabled" : "disabled");
	i->sub_elements[0].id = BTN_TOGGLE;
	i->sub_elements[0].outline_thickness = 2.f;

	// Button 2: Accept
	i->sub_elements[1].bounds = (Rectangle){100, 300, 150, 50};
	sprintf(i->sub_elements[1].text, "OK");
	i->sub_elements[1].id = BTN_ACCEPT;
	i->sub_elements[1].outline_thickness = 2.f;
}

void init_menu_conf(DisplayConfigureInfo *i) {
	i->menu_element_count = 4;
	int screen_width = GetScreenWidth();
	int screen_height = GetScreenHeight();

	int const border_fraction_screen_width = 5;
	int const border_fraction_screen_height = 15;
	int rectangle_height = screen_height / 20;
	int rectangle_width = (border_fraction_screen_width - 2) *
			      screen_width / border_fraction_screen_width;
	int rectangle_y_base = screen_height / border_fraction_screen_height;
	int rectangle_height_spacing =
	    screen_height / (i->menu_element_count + 1);
	int rectangle_x = screen_width / border_fraction_screen_width;

	const char *labels[] = {"Board width", "Board height", "Snake speed",
				"Board wrapping"};

	// assign rectangle boxes
	for (int idx = 0; idx < i->menu_element_count; idx++) {
		i->menu_elements[idx].bounds.x = rectangle_x;
		i->menu_elements[idx].bounds.y =
		    idx * rectangle_height_spacing + rectangle_y_base;
		i->menu_elements[idx].bounds.width = rectangle_width;
		i->menu_elements[idx].bounds.height = rectangle_height;
		sprintf(i->menu_elements[idx].text, "%s", labels[idx]);
	}

	// init play button
	i->menu_elements[i->menu_element_count].outline_thickness = 2.f;
	sprintf(i->menu_elements[i->menu_element_count].text, "Play");
	int const rectangle_thickness_lines = 2;
	int button_width = MeasureText(
	    i->menu_elements[i->menu_element_count].text, rectangle_height);
	i->menu_elements[i->menu_element_count].bounds.x =
	    screen_width - rectangle_x - button_width;
	i->menu_elements[i->menu_element_count].bounds.y =
	    i->menu_element_count * rectangle_height_spacing + rectangle_y_base;
	i->menu_elements[i->menu_element_count].bounds.width = button_width;
	i->menu_elements[i->menu_element_count].bounds.height =
	    rectangle_height;
}

void init_conf(GameConfigureState state_conf, DisplayConfigureInfo *info) {
	switch (state_conf) {
	case STATE_CONFIGURE_MENU:
		init_menu_conf(info);
	case STATE_CONFIGURE_WIDTH:
		init_width_conf(info);
		break;
	case STATE_CONFIGURE_HEIGHT:
		init_height_conf(info);
		break;
	case STATE_CONFIGURE_SNAKE_SPEED:
		init_speed_conf(info);
		break;
	case STATE_CONFIGURE_WRAPPING:
		init_wrapping_conf(info);
		break;
	}
}
