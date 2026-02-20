#include "board.h"
#include "debug.h"
#include <stdio.h>
#ifdef _WIN32
#else
#include <sys/ioctl.h>
#endif
#include <unistd.h>

int term_rows, term_colums;

void board_draw(const Board *b) {
	const int board_width = board_get_width(b);
	const int board_height = board_get_height(b);

	// int x_padding = board_width + 2;

	// move cursor to top of screen and overwrite
	printf("\033[H");
	// draw top wall
	for (int x = 0; x < board_width + 2; x++)
		putchar('#');
	putchar('\n');
	for (int y = 0; y < board_height; y++) {
		// left wall
		putchar('#');
		// squares
		for (int x = 0; x < board_width; x++) {
			putchar(board_get_square(b, x, y));
		}
		// right wall
		putchar('#');
		putchar('\n');
	}
	// bottom wall
	for (int x = 0; x < board_width + 2; x++)
		putchar('#');
	putchar('\n');
	LogDebug("board was drawn\n");
}

void term_init() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	term_rows = w.ws_row;
	term_colums = w.ws_col;
}

void term_get_offset(const int width, const int length, int *offset_row,
		     int *offset_colums) {
	*offset_row = (term_rows - length) / 2;
	*offset_colums = (term_colums - width) / 2;
}
