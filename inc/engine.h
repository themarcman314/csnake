#ifndef ENGINE
#define ENGINE
#include "board.h"
#include "timer.h"

InputKey get_key(void);
void board_draw(Board const *b, int const score);
void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y);

void engine_init();
void display_end(int score);
void display_welcome();
void display_configure(bool const is_configured_width,
		       bool const is_configured_height, float const freq,
		       int const width, int const height);

void window_get_size();

void window_periodic_start();
void window_periodic_end();

#endif
