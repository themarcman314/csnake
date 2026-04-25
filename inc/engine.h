#ifndef ENGINE
#define ENGINE
#include "board.h"
#include "game.h"
#include "raylib.h"
#include "timer.h"

typedef struct DrawingParameters DrawingParameters;

typedef struct {
	Board *demo;
	GameConfigureState state_conf;
	GameConfigureSelectedState state_select;
	float freq;
	int width;
	int height;
	char *name;
} DisplayConfigureInfo;

void board_draw(Board const *b, int const score, bool show_score);
void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y);
bool board_check_collisions(Board const *b);
bool board_check_edge(Board const *b);
void engine_init();
void display_end(Board const *b, int const score, int game_over_timestamp);
void display_welcome();
void display_configure(DisplayConfigureInfo const info);

void window_get_size();

void window_periodic_start();
void window_periodic_end();

void set_keyboard_type();

void draw_square(DrawingParameters const *p, int const x, int const y, Color c);

void set_start_coords_grid(int grid_width, int grid_height);

#endif
