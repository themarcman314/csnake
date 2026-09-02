#pragma once
#include "board.h"
#include "configure_types.h"
#include "raylib.h"
#include "score.h"
#include "timer.h"
#include <stdint.h>

typedef struct DrawingParameters DrawingParameters;

void get_screen_measurements();
typedef void (*ConfDisplayFunc)(DisplayConfigureInfo info);
void display_menu_conf(DisplayConfigureInfo info);
void display_name_conf(DisplayConfigureInfo info);
void display_width_conf(DisplayConfigureInfo info);
void display_height_conf(DisplayConfigureInfo info);
void display_snake_speed_conf(DisplayConfigureInfo info);
void display_wrap_conf(DisplayConfigureInfo info);
void display_wrapping_conf(DisplayConfigureInfo info);
bool is_display_name_box_overflown(char *name);

void board_draw(Board const *b, int score, bool is_draw_game_over,
		bool show_score);

void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y);
bool board_check_collisions(Board const *b);
void engine_init();
void display_end(Board const *b, int const score, int game_over_timestamp);
void display_welcome();
void display_configure(DisplayConfigureInfo const info);
void display_high_score(HighScoreEntry const *h, int const num_entries,
			Texture2D *flags);

void window_periodic_start();
void window_periodic_end();

void set_keyboard_type();

void draw_square(DrawingParameters const *p, int const x, int const y, Color c);

UIElement CreateButton(float x, float y, float width, float height, char *text);

void set_start_coords_grid(int grid_width, int grid_height);
