#include <stdio.h>
#include "board.h"

void board_draw(const Board *b) {
	const int board_width = board_get_width(b);
	const int board_height = board_get_height(b);

	// move cursor to top of screen and overwrite
	printf("\033[H");
	// draw top wall
	for (int x = 0; x < board_width + 2; x++) putchar('#');
	putchar('\n');
	for(int y = 0; y < board_height; y++) {
		// left wall
		putchar('#');
		// squares
		for(int x = 0; x < board_width; x++) {
			putchar(board_get_square(b, x, y));
		}
		// right wall
		putchar('#'); putchar('\n');
	}
	// bottom wall
	for (int x = 0; x < board_width + 2; x++) putchar('#');
	putchar('\n');
}
