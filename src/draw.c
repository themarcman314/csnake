#include "board.h"
#include "debug.h"
#include <stdio.h>
#ifdef _WIN32
#else
#include <sys/ioctl.h>
#endif
#include <unistd.h>

int term_rows, term_colums;

void static term_board_draw(const Board *b);
void term_clear_quick(void);
void term_clear_full(void);
void term_get_offset(int const width, int const length, int *offset_rows,
		     int *offset_colums);
void term_print_spaces(int const num);
void term_print_newlines(int const num);
void term_init();

void board_draw(Board const *b) {
#ifdef TERM_SIMPLE
	term_board_draw(b);
#elif TERM_NCURSES
#elif GRAPHICAL
#endif
}

void static term_board_draw(Board const *b) {
	int const board_width = b->width;
	int const board_height = b->height;

	term_clear_full();

	int offset_rows, offset_colums;
	term_get_offset(board_width, board_height, &offset_rows,
			&offset_colums);


	term_print_newlines(offset_rows);

	// draw top wall
	term_print_spaces(offset_colums);
	for (int x = 0; x < board_width + 2; x++)
		putchar('#');
	putchar('\n');
	for (int y = 0; y < board_height; y++) {
		// left wall
		term_print_spaces(offset_colums);
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
	term_print_spaces(offset_colums);
	for (int x = 0; x < board_width + 2; x++)
		putchar('#');
	putchar('\n');
	LogDebug("board was drawn\n");
}

void term_print_spaces(int const num) {
	for (int i = 0; i < num; i++)
		putchar(' ');
}
void term_print_newlines(int const num) {
	for (int i = 0; i < num; i++)
		putchar('\n');
}

void term_init() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	term_rows = w.ws_row;
	term_colums = w.ws_col;
}

void term_print_size(void) {
	printf("rows: %d\ncolumns: %d\n", term_rows, term_colums);
}

void term_get_offset(int const width, int const height, int *offset_rows,
		     int *offset_colums) {
	*offset_rows = (term_rows - height) / 2;
	*offset_colums = (term_colums - width) / 2;
}

void term_clear_quick(void) {
	// move cursor to top of screen and overwrite
	printf("\033[H");
}
void term_clear_full(void) {
	// move cursor to top and clear from cursor
	printf("\033[H\033[J");
}
