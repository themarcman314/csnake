#include <stdlib.h>
#include <stdio.h>
#include "board.h"

struct Board{
	int width;
	int height;
	char *squares;
};

Board *board_create(const int width, const int height) {
	Board *b = malloc(sizeof(Board));
	b->height = height; b->width = width;
	int board_size = b->width * b->height * sizeof(char);
	b->squares = (char *)calloc(1, board_size); // allocate board and init to 0
	return b;
}

int board_get_width(const Board *b) {
	return b->width;
}

int board_get_height(const Board *b) {
	return b->height;
}

char board_get_square(const Board *b, const int x, const int y) {
	return b->squares[y*b->width + x];
}

void board_set_square(Board *b, const int x, const int y, const char c) {
	b->squares[y*b->width + x] = c;
}

void board_print_info(const Board *b) {
	printf("width: %d\nheigth: %d\n", b->width, b->height);
}

