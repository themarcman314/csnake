#include <stdlib.h>
#include <stdio.h>
#include "board.h"

struct Board{
	int width;
	int height;
	char *squares;
};

Board *board_create(int width, int height) {
	Board *b = malloc(sizeof(Board));
	b->height = height; b->width = width;
	b->squares = (char *)malloc(b->width * b->height * sizeof(char));

	for (int i = 0; i < b->height * b->width; i++) {
		b->squares[i] = '-';
	}
	b->squares[0] = '#';
	b->squares[4] = '#';
	b->squares[(b->height-1)] = '#';
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

void board_print_info(const Board *b) {
	printf("width: %d\nheigth: %d\n", b->width, b->height);
}
