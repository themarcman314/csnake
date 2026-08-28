#pragma once
// #include "engine.h"
// #include "engine.h"
#include "raylib.h"
#include <stdbool.h>

typedef enum {
	STATE_CONFIGURE_NAME,
	STATE_CONFIGURE_MENU,
	STATE_CONFIGURE_WIDTH,
	STATE_CONFIGURE_HEIGHT,
	STATE_CONFIGURE_SNAKE_SPEED,
	STATE_CONFIGURE_WRAPPING,
	STATE_CONFIGURE_APPLY,
} GameConfigureState;

typedef enum {
	STATE_CONFIGURE_SELECTED_WIDTH,
	STATE_CONFIGURE_SELECTED_HEIGHT,
	STATE_CONFIGURE_SELECTED_SNAKE_SPEED,
	STATE_CONFIGURE_SELECTED_WRAPPING,
	STATE_CONFIGURE_SELECTED_PLAY,
	STATE_CONFIGURE_SELECTED_MENU,
	STATE_CONFIGURE_SELECTED_NONE,
} GameConfigureSelectedState;

typedef struct {
	GameConfigureState current_state;
	GameConfigureSelectedState selected_item;
	KeyboardKey input_key;
	GameConfigureState next_state;
} GameConfigureStateTransition;

typedef enum {
	BTN_NONE,
	BTN_DECREASE,
	BTN_INCREASE,
	BTN_ACCEPT,
	BTN_TOGGLE,
	BTN_CANCEL,
} ElementID;

typedef struct {
	Rectangle bounds;
	float outline_thickness;
	bool is_hovered;
	char text[20];
	ElementID id;
} UIElement;

typedef struct Board Board;

typedef struct {
	Board *demo;
	GameConfigureSelectedState state_select;
	float freq;
	int width;
	int height;
	bool board_wrapping;
	char *name;
	UIElement menu_elements[5];
	UIElement sub_elements[5];
	int menu_element_count;
	int sub_element_count;
	Vector2 last_mouse_pos;
} DisplayConfigureInfo;

/* Group both shared items into a single container */
typedef struct {
	GameConfigureState state_conf;
	DisplayConfigureInfo info;
} GameConfigureContext;
