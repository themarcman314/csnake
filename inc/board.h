#ifndef BOARD
#define BOARD

#include "input.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct Snake Snake;
typedef struct Food Food;

typedef struct {
	int width, height;
	Snake *s;
	Food *f;
	char *squares;
} Board;

Board *board_create(const int width, const int height);
void board_destroy(Board **b);
void board_print_info(const Board *b);
int board_get_width(const Board *b);
int board_get_height(const Board *b);
bool board_check_all_collisions(const Board *b);
void board_set_square(Board *b, const int x, const int y, const char c);
char board_get_square(const Board *b, const int x, const int y);
void board_update(Board *b);

bool snake_ate_food(Snake *s, Food *f);
void snake_segment_add(Snake *s);
void snake_create(Board *b);
void snake_init(Board *b);
void snake_kill(Snake **p_s);
void snake_get_head_position(Snake const *s, int *x, int *y);
void snake_update_square_position(Snake *s, int const b_width,
				  int const b_height, bool wrapping_enabled);
void snake_head_direction_set_next(Snake *const s, Input const key);
void snake_head_direction_set(Snake *s);
void snake_right_direction_to_current(Snake *s);

void food_init(Board *b);
void food_spawn(Board *b);
void food_destroy(Food **f);

#endif
