#include <stdio.h>
#include "board.h"

void board_draw(const Board *b) {
	for(int y = 0; y < board_get_height(b); y++) {
		for(int x = 0; x < board_get_width(b); x++) {
			printf("%c", board_get_square(b, x, y));
		}
		printf("\n");
	}
}
