#pragma once
#include "board.h"
#include "engine.h"
#include "game.h"

void snake_demo(Board *demo_b, float freq, bool board_wrapping);

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

void init_width_conf(DisplayConfigureInfo *i);
void init_height_conf(DisplayConfigureInfo *i);
void init_speed_conf(DisplayConfigureInfo *i);
void init_wrapping_conf(DisplayConfigureInfo *i);
void init_menu_conf(DisplayConfigureInfo *i);

int update_name_conf(Game *g, DisplayConfigureInfo *i);
int update_menu_conf(Game *g, DisplayConfigureInfo *i);
int update_wrapping_conf(Game *g, DisplayConfigureInfo *i);
int update_width_conf(Game *g, DisplayConfigureInfo *i);
int update_height_conf(Game *g, DisplayConfigureInfo *i);
int update_snake_speed_conf(Game *g, DisplayConfigureInfo *i);

typedef int (*GameConfigureFunc)(Game *g, DisplayConfigureInfo *i);
static const GameConfigureFunc conf_logic_funcs[] = {
    [STATE_CONFIGURE_NAME] = update_name_conf,
    [STATE_CONFIGURE_MENU] = update_menu_conf,
    [STATE_CONFIGURE_WIDTH] = update_width_conf,
    [STATE_CONFIGURE_HEIGHT] = update_height_conf,
    [STATE_CONFIGURE_WRAPPING] = update_wrapping_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = update_snake_speed_conf,
};
static const ConfDisplayFunc conf_display_funcs[] = {
    [STATE_CONFIGURE_NAME] = display_name_conf,
    [STATE_CONFIGURE_MENU] = display_menu_conf,
    [STATE_CONFIGURE_WIDTH] = display_width_conf,
    [STATE_CONFIGURE_HEIGHT] = display_height_conf,
    [STATE_CONFIGURE_SNAKE_SPEED] = display_snake_speed_conf,
    [STATE_CONFIGURE_WRAPPING] = display_wrapping_conf,
};
