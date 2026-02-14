#include <stdio.h>
#include <stdlib.h>
#include "board.h"

struct Board{
	int width;
	int height;
	char *squares;
};

Board board_create(int width, int height) {
	Board b;
	b.height = height; b.width = width;
	b.squares = (char *)malloc(b.width * b.height * sizeof(char));
	return b;
}

void board_print(Board b) {
	for(int w = 0; w < b.width; w++) {
		printf("%c", b.squares[w]);
	}
	printf("\n");
}
