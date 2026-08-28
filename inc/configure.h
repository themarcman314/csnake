#pragma once
#include "board.h"
#include "configure_types.h"

typedef struct Game Game;

void snake_demo(Board *demo_b, float freq, bool board_wrapping);

void init_width_conf(DisplayConfigureInfo *i);
void init_height_conf(DisplayConfigureInfo *i);
void init_speed_conf(DisplayConfigureInfo *i);
void init_wrapping_conf(DisplayConfigureInfo *i);
void init_menu_conf(DisplayConfigureInfo *i);
void init_conf(GameConfigureState state_conf, DisplayConfigureInfo *info);

int update_name_conf(Game *g, DisplayConfigureInfo *i);
int update_menu_conf(Game *g, DisplayConfigureInfo *i);
int update_wrapping_conf(Game *g, DisplayConfigureInfo *i);
int update_width_conf(Game *g, DisplayConfigureInfo *i);
int update_height_conf(Game *g, DisplayConfigureInfo *i);
int update_snake_speed_conf(Game *g, DisplayConfigureInfo *i);

typedef int (*GameConfigureFunc)(Game *g, DisplayConfigureInfo *i);
