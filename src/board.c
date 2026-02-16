#include "board.h"
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	SNAKE_UP,
	SNAKE_DOWN,
	SNAKE_LEFT,
	SNAKE_RIGHT,
} Direction;

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
	char *squares;
};

TermInputKey term_get_key(void);
void board_snake_init(Board *b);
SnakeSegment *snake_segment_create(const int x, const int y);

Board *board_create(const int width, const int height) {
	Board *b = malloc(sizeof(Board));
	if (b == NULL) {
		goto fail_board;
	}
	b->height = height;
	b->width = width;
	board_snake_init(b);
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
void snake_segment_find_new_coords(const Snake *s, int x, int y) {
	assert(0 && "not implemented");
}

void snake_segment_add(Snake *s) {
	SnakeSegment *current;
	int x, y;
	current = s->head;
	assert(s->head != NULL);
	while (current != NULL) {
		current = current->child;
	}
	snake_segment_find_new_coords(s, x, y);
	snake_segment_create(x, y);
}

void snake_update(Snake *s) {
	SnakeSegment *current;
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
		current->child->x = current->x;
		current->child->y = current->y;
	}
}

bool board_check_collisions(Board *b) {
	// snake against board
	if (b->s.head->x < 0 || b->s.head->x >= b->width || b->s.head->y < 0 ||
	    b->s.head->y >= b->height) {
		return true;
	} else {
		return false;
	}
}

void board_update(Board *b) {
	snake_update(&b->s);
	memset(b->squares, ' ', b->width * b->height);
	SnakeSegment *current_seg = b->s.head;
	board_set_square(b, current_seg->x, current_seg->y, '@');
	current_seg = current_seg->child;
	while (current_seg != NULL) {
		board_set_square(b, current_seg->x, current_seg->y, '+');
		current_seg = current_seg->child;
	}
}
