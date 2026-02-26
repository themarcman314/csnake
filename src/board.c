#include "board.h"
#include "debug.h"
#include "input.h"
#include "conf.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
	SNAKE_NONE,
	SNAKE_UP,
	SNAKE_DOWN,
	SNAKE_LEFT,
	SNAKE_RIGHT,
} Direction;

#define SNAKE_DIR_QUEUE_SIZE 3

struct Food {
	int x, y;
};

typedef struct SnakeSegment SnakeSegment;
typedef struct HeadDirQueue HeadDirQueue;

struct SnakeSegment {
	int x, y;
	SnakeSegment *child;
};

/*
 * Allows us to store consecutive directions in a tick.
 * This makes the user experience better as several pending directions can be
 * queued per tick. otherwise user input is limited to once per tick leading to
 * a less responsive feel
 */
struct HeadDirQueue {
	Direction head_dir_next[SNAKE_DIR_QUEUE_SIZE];
	unsigned index_write;
	unsigned index_read;
};

struct Snake {
	SnakeSegment *head;
	int length;
	HeadDirQueue queue;
	Direction head_dir_current;
};

void snake_print(Snake const *s);
void snake_create(Board *b);
SnakeSegment *snake_segment_create(int const x, int const y);
void food_set_square(Food *f, int const x, int const y);
bool snake_check_collisions(Snake const *s);
bool board_check_collisions(Board const *b);
static bool snake_head_direction_is_opposite(Direction const a,
					     Direction const b);
Direction snake_head_direction_translate_from_input(TermInputKey const key,
						    Direction const current);

Board *board_create(int const width, int const height) {
	Board *b = malloc(sizeof(Board));
	if (b == NULL) {
		goto fail_board;
	}
	b->height = height;
	b->width = width;
	int board_size = b->width * b->height * sizeof(char);
	b->squares =
	    (char *)calloc(1, board_size); // allocate board and init to 0
	if (b->squares == NULL) {
		goto fail_squares;
	}

	snake_create(b);
	food_init(b);
	return b;

fail_squares:
	board_destroy(&b);
fail_board:
	fprintf(stderr, "Unable to allocate memory for board");
	exit(EXIT_FAILURE);
}

void snake_kill(Snake **p_s) {
	if (!p_s || !*p_s)
		return;
	Snake *s = *p_s;
	LogDebug("Killing snake");
	while (s->head != NULL) {
		SnakeSegment *tmp = s->head;
		s->head = s->head->child;
		free(tmp);
		tmp = NULL;
	}
	free(s);
	*p_s = NULL;
}
void food_destroy(Food **f) {
	if (f || *f) {
		free(*f);
		*f = NULL;
	}
}
void board_destroy(Board **p_b) {
	if (p_b || *p_b) {
		LogDebug("Destroying Board");
		Board *b = *p_b;
		free(b->squares);
		free(b);
		*p_b = NULL;
	}
}

int board_get_width(Board const *b) { return b->width; }

int board_get_height(Board const *b) { return b->height; }

char board_get_square(Board const *b, int const x, int const y) {
	return b->squares[y * b->width + x];
}

void board_set_square(Board *b, int const x, int const y, char const c) {
	b->squares[y * b->width + x] = c;
}

void board_print_info(Board const *b) {
	printf("width: %d\nheigth: %d\n", b->width, b->height);
}

/*
 * Initialize somewhere on the board
 */
void snake_create(Board *b) {
	b->s = malloc(sizeof(Snake));
	if (b->s == NULL) {
		goto snake_create_failed;
	}
	snake_init(b);

	b->f = malloc(sizeof(Food));
	if (b->f == NULL) {
		goto food_create_failed;
	}
	return;

food_create_failed:
	snake_kill(&b->s);
snake_create_failed:
	board_destroy(&b);
	exit(EXIT_FAILURE);
}

void snake_init(Board *b) {
	b->s->head = snake_segment_create(b->width / 2, b->height / 2);
	memset(b->s->queue.head_dir_next, SNAKE_NONE, SNAKE_DIR_QUEUE_SIZE);
	b->s->head_dir_current = SNAKE_NONE;
	b->s->queue.index_write = 0;
	b->s->queue.index_read = 0;
	b->s->length = 1;
}

SnakeSegment *snake_segment_create(const int x, const int y) {
	SnakeSegment *s = malloc(sizeof(SnakeSegment));
	if (s != NULL) {
		s->child = NULL;
		s->x = x;
		s->y = y;
	} else {
		char const err_msg[] = "Issue creating SnakeSegment node\n";
		fprintf(stderr, err_msg);
		LogDebug(err_msg);
		exit(EXIT_FAILURE);
	}
	return s;
}

void snake_head_direction_set_next(Snake *const s, TermInputKey const key) {
	if (key == IN_NONE) {
		return;
	}
	Direction dir_last;
	Direction dir_current =
	    snake_head_direction_translate_from_input(key, s->head_dir_current);
	// get last direction
	if (s->queue.index_write != s->queue.index_read) { // not empty
		unsigned index_last =
		    (s->queue.index_write + SNAKE_DIR_QUEUE_SIZE - 1) %
		    SNAKE_DIR_QUEUE_SIZE;
		dir_last = s->queue.head_dir_next[index_last];
	} else { // empty
		dir_last = s->head_dir_current;
	}
	// ignore 180° reversals in direction
	if (!snake_head_direction_is_opposite(dir_last, dir_current)) {
		unsigned index_next =
		    (s->queue.index_write + 1) % SNAKE_DIR_QUEUE_SIZE;
		if (index_next != s->queue.index_read) { // not full
			s->queue.head_dir_next[s->queue.index_write] =
			    dir_current;
			s->queue.index_write = index_next;
		}
	}
}

static bool snake_head_direction_is_opposite(Direction const a,
					     Direction const b) {
	return (a == SNAKE_UP && b == SNAKE_DOWN) ||
	       (a == SNAKE_DOWN && b == SNAKE_UP) ||
	       (a == SNAKE_LEFT && b == SNAKE_RIGHT) ||
	       (a == SNAKE_RIGHT && b == SNAKE_LEFT);
}

Direction snake_head_direction_translate_from_input(TermInputKey const key,
						    Direction const current) {
	switch (key) {
	case IN_UP:
		return SNAKE_UP;
	case IN_DOWN:
		return SNAKE_DOWN;
	case IN_LEFT:
		return SNAKE_LEFT;
	case IN_RIGHT:
		return SNAKE_RIGHT;
	default:
		return current;
	}
}

void snake_head_direction_set(Snake *s) {
	if (s->queue.index_write != s->queue.index_read) {
		s->head_dir_current =
		    s->queue.head_dir_next[s->queue.index_read];
		s->queue.index_read =
		    (s->queue.index_read + 1) % SNAKE_DIR_QUEUE_SIZE;
	}
}

/*
 * Given that there are only 3 possible tiles where
 * the latest snake segment can be added,
 * I choose to add it to the opposite direction of the snake body.
 * If n-1 is NORTH of n, then n+1 will go SOUTH.
 * Similarily if n-1 is WEST of n, then n+1 will go EAST
 */
void snake_segment_find_new_coords(Snake const *s, int *x_new, int *y_new) {
	// assert(0 && "not implemented");
	if (s->head->child == NULL) {
		switch (s->head_dir_current) {
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
		default:
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
	}
	LogDebug("new coords:\nx= %d\ny= %d", *x_new, *y_new);
}

void snake_segment_add(Snake *s) {
	SnakeSegment *current = s->head;
	int x, y = 0;
	// assert(s->head->child == NULL);
	while (current->child != NULL) {
		current = current->child;
	}
	snake_segment_find_new_coords(s, &x, &y);
	current->child = snake_segment_create(x, y);
	s->length++;
}

void snake_update_square_position(Snake *s) {
	SnakeSegment *current = s->head;
	int x[s->length];
	int y[s->length];
	// save to temp arrays
	for (int i = 0; i < s->length; i++) {
		x[i] = current->x;
		y[i] = current->y;
		current = current->child;
		LogDebug("before update");
		snake_print(s);
	}
	switch (s->head_dir_current) {
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
	case SNAKE_NONE:
		break;
	}
	current = s->head->child;
	// set new coords from temp array
	for (int i = 1; i < s->length; i++) {
		current->x = x[i - 1];
		current->y = y[i - 1];
		current = current->child;
		LogDebug("after update");
		snake_print(s);
	}
}

void food_init(Board *b) {
	srand(time(NULL));
	food_spawn(b);
}

void food_set_square(Food *f, int const x, int const y) {
	f->x = x;
	f->y = y;
}

void food_spawn(Board *b) {
	// set to width instead of width + 1 since array starts at 0
	int const x = rand() % b->width;
	int const y = rand() % b->height;
	food_set_square(b->f, x, y);
	LogDebug("food spawned");
}

bool board_check_all_collisions(Board const *b) {
	if (board_check_collisions(b) || snake_check_collisions(b->s)) {
		return true;
	}
	return false;
}

void snake_get_head_position(Snake const *s, int *x, int *y) {
	*x = s->head->x;
	*y = s->head->y;
}

bool snake_check_collisions(Snake const *s) {
	SnakeSegment *current = s->head->child;
	while (current != NULL) {
		if (s->head->x == current->x && s->head->y == current->y) {
			return true;
		}
		current = current->child;
	}
	return false;
}

bool board_check_collisions(Board const *b) {
	// snake against board
	if (b->s->head->x < 0 || b->s->head->x >= b->width ||
	    b->s->head->y < 0 || b->s->head->y >= b->height) {
		return true;
	} else {
		return false;
	}
	LogDebug("checked board collisions");
}

bool snake_ate_food(Snake *s, Food *f) {
	if (s->head->x == f->x && s->head->y == f->y)
		return true;
	return false;
}

void board_update(Board *b) {
	memset(b->squares, ' ', b->width * b->height);
	board_set_square(b, b->f->x, b->f->y, FOOD_CHAR);
	SnakeSegment *current_seg = b->s->head;
	board_set_square(b, current_seg->x, current_seg->y, SNAKE_HEAD_CHAR);
	while (current_seg->child != NULL) {
		current_seg = current_seg->child;
		board_set_square(b, current_seg->x, current_seg->y, SNAKE_BODY_CHAR);
	}
	LogDebug("updated board");
}

void snake_print(Snake const *s) {
	SnakeSegment *current = s->head;
	LogDebug("snake:");
	while (current != NULL) {
		LogDebug("x: %d y: %d", current->x, current->y);
		current = current->child;
	}
}
