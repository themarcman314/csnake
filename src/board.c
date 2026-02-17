#include "board.h"
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
	SNAKE_UP,
	SNAKE_DOWN,
	SNAKE_LEFT,
	SNAKE_RIGHT,
} Direction;

struct Food {
	int x, y;
};

struct SnakeSegment {
	int x, y;
	SnakeSegment *child;
};

struct Snake {
	SnakeSegment *head;
	int length;
	Direction head_dir;
};

struct Board {
	int width, height;
	Snake s;
	Food f;
	char *squares;
};

TermInputKey term_get_key(void);
void board_snake_init(Board *b);
SnakeSegment *snake_segment_create(const int x, const int y);
void food_spawn(Board *b);
void food_set_square(Food *f, const int x, const int y);
void snake_segment_add(Snake *s);

Board *board_create(const int width, const int height) {
	Board *b = malloc(sizeof(Board));
	if (b == NULL) {
		goto fail_board;
	}
	b->height = height;
	b->width = width;
	board_snake_init(b);
	food_init(b);
	int board_size = b->width * b->height * sizeof(char);
	b->squares =
	    (char *)calloc(1, board_size); // allocate board and init to 0
	if (b->squares == NULL) {
		goto fail_squares;
	}
	return b;

fail_squares:
	free(b);
fail_board:
	fprintf(stderr, "Unable to allocate memory for board");
	exit(EXIT_FAILURE);
}

void board_destroy(Board *b) { free(b); }

int board_get_width(const Board *b) { return b->width; }

int board_get_height(const Board *b) { return b->height; }

char board_get_square(const Board *b, const int x, const int y) {
	return b->squares[y * b->width + x];
}

void board_set_square(Board *b, const int x, const int y, const char c) {
	b->squares[y * b->width + x] = c;
}

void board_print_info(const Board *b) {
	printf("width: %d\nheigth: %d\n", b->width, b->height);
}

/*
 * Initialize somewhere on the board
 */
void board_snake_init(Board *b) {
	b->s.head = snake_segment_create(b->width / 2, b->height / 2);
	b->s.head_dir = SNAKE_UP;
	b->s.length = 0;
}

SnakeSegment *snake_segment_create(const int x, const int y) {
	SnakeSegment *s = malloc(sizeof(SnakeSegment));
	s->child = NULL;
	s->x = x;
	s->y = y;
	return s;
}

void snake_head_set_direction(Board *b) {
	switch (term_get_key()) {
	case IN_UP:
		b->s.head_dir = SNAKE_UP;
		break;
	case IN_DOWN:
		b->s.head_dir = SNAKE_DOWN;
		break;
	case IN_LEFT:
		b->s.head_dir = SNAKE_LEFT;
		break;
	case IN_RIGHT:
		b->s.head_dir = SNAKE_RIGHT;
		break;
	case IN_NONE:
		break;
	}
}

/*
 * Given that there are only 3 possible tiles where
 * the latest snake segment can be added,
 * I choose to add it to the opposite direction of the snake body.
 * If n-1 is NORTH of n, then n+1 will go SOUTH.
 * Similarily if n-1 is WEST of n, then n+1 will go EAST
 */
void snake_segment_find_new_coords(const Snake *s, int *x_new, int *y_new) {
	// assert(0 && "not implemented");
	if (s->head->child == NULL) {
		printf("creating 2nd node...\n");
		switch (s->head_dir) {
		case SNAKE_UP:
			*x_new = s->head->x;
			*y_new = s->head->y + 1;
			break;
		case SNAKE_DOWN:
			*x_new = s->head->x;
			*y_new = s->head->y - 1;
			break;
		case SNAKE_LEFT:
			*x_new = s->head->x + 1;
			*y_new = s->head->y;
			break;
		case SNAKE_RIGHT:
			*x_new = s->head->x - 1;
			*y_new = s->head->y;
			break;
		}
	} else {
		SnakeSegment *current = s->head;
		while (current->child->child != NULL) {
			current = current->child;
		}
		int x_diff = current->x - current->child->x;
		int y_diff = current->y - current->child->y;
		if (y_diff == 1) {
			*x_new = current->child->x;
			*y_new = current->child->y + 1;
		} else if (y_diff == -1) {
			*x_new = current->child->x;
			*y_new = current->child->y - 1;
		} else if (x_diff == 1) {
			*x_new = current->child->x - 1;
			*y_new = current->child->y;
		} else if (x_diff == -1) {
			*x_new = current->child->x + 1;
			*y_new = current->child->y;
		}
		printf("new coords:\nx= %d\ny= %d\n", *x_new, *y_new);
	}
}

void snake_segment_add(Snake *s) {
	s->length++;
	SnakeSegment *current;
	int x, y = 0;
	current = s->head;
	assert(s->head != NULL);
	while (current->child != NULL) {
		current = current->child;
	}
	snake_segment_find_new_coords(s, &x, &y);
	current->child = snake_segment_create(x, y);
}

// work your way back instead of forward
void snake_update_square_position(Snake *s) {
	SnakeSegment *current;
	SnakeSegment coords;
	SnakeSegment next_coords;
	coords.x = s->head->x;
	coords.y = s->head->y;
	switch (s->head_dir) {
	case SNAKE_UP:
		s->head->y--;
		break;
	case SNAKE_DOWN:
		s->head->y++;
		break;
	case SNAKE_LEFT:
		s->head->x--;
		break;
	case SNAKE_RIGHT:
		s->head->x++;
		break;
	}
	current = s->head;
	while (current->child != NULL) {
		coords.x = current->child->x;
		next_coords.y = current->child->y;
		current->child->x = next_coords.x;
		current->child->y = next_coords.y;
		current = current->child;
	}
}

void food_init(Board *b) {
	srand(time(NULL));
	food_spawn(b);
}

void food_set_square(Food *f, const int x, const int y) {
	f->x = x;
	f->y = y;
}

void food_spawn(Board *b) {
	// set to width instead of width + 1 since array starts at 0
	int x = rand() % b->width;
	int y = rand() % b->height;
	food_set_square(&b->f, x, y);
}

void board_check_all_collisions(const Board *B) {}

bool board_check_collisions(const Board *b) {
	// snake against board
	if (b->s.head->x < 0 || b->s.head->x >= b->width || b->s.head->y < 0 ||
	    b->s.head->y >= b->height) {
		return true;
	} else {
		return false;
	}
}

bool snake_ate_food(Snake *s, Food *f) {
	if (s->head->x == f->x && s->head->y == f->y)
		return true;
	return false;
}

void board_update(Board *b) {
	snake_update_square_position(&b->s);
	if (snake_ate_food(&b->s, &b->f)) {
		snake_segment_add(&b->s);
		food_spawn(b);
	}
	memset(b->squares, ' ', b->width * b->height);
	board_set_square(b, b->f.x, b->f.y, '*');
	SnakeSegment *current_seg = b->s.head;
	board_set_square(b, current_seg->x, current_seg->y, '@');
	while (current_seg->child != NULL) {
		static int stuff = 0;
		current_seg = current_seg->child;
		board_set_square(b, current_seg->x, current_seg->y, '+');
	}
}
