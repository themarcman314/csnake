#include "term.h"
#include "ansi.h"
#include "conf.h"
#include "debug.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

int term_rows, term_colums;

static int count_digits(int num);

void board_draw(Board const *const b, int const score) {
	int const board_width = b->width;
	int const board_height = b->height;

	term_clear_full();

	int offset_rows, offset_colums;
	term_get_offset(board_width, board_height, &offset_rows,
			&offset_colums);

	// offset
	printf("\033[%d;%dH", --offset_rows,
	       offset_colums + board_width - 6 - count_digits(score) + 1);
	printf("score: %s%d", MAG, score);
	printf("\033[%d;%dH", ++offset_rows, offset_colums);

	// draw top wall
	term_color_set(BOARD_WALL_COLOR);
	for (int x = 0; x < board_width + 2; x++) {
		putchar('#');
	}
	term_color_clear();

	for (int y = 0; y < board_height; y++) {
		// left wall
		offset_rows++;
		printf("\033[%d;%dH", offset_rows, offset_colums);
		// term_print_spaces(offset_colums);
		term_color_set(BOARD_WALL_COLOR);
		putchar('#');
		term_color_clear();
		// squares
		for (int x = 0; x < board_width; x++) {
			char square = board_get_square(b, x, y);
			if (square == SNAKE_BODY_CHAR ||
			    square == SNAKE_HEAD_CHAR)
				term_color_set(SNAKE_COLOR);
			else if (square == FOOD_CHAR) {
				term_color_set(FOOD_COLOR);
			}
			putchar(square);
			term_color_clear();
		}
		// right wall
		term_color_set(BOARD_WALL_COLOR);
		putchar('#');
		term_color_clear();
		putchar('\n');
	}
	// bottom wall
	printf("\033[%d;%dH", ++offset_rows, offset_colums);
	term_color_set(BOARD_WALL_COLOR);
	for (int x = 0; x < board_width + 2; x++) {
		putchar('#');
	}
	putchar('\n');
	LogDebug("board was drawn\n");
}

void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y) {
	// find board offset
	int const height = board_get_height(b);
	int const width = board_get_width(b);
	int offset_rows, offset_colums;
	term_get_offset(width, height, &offset_rows, &offset_colums);
	// draw an X at that position
	printf("\033[%d;%dH", board_y + offset_rows + 1,
	       board_x + offset_colums + 1);
	term_color_set(TERM_RED);
	putchar('X');
	term_color_clear();
	term_clear_quick();
	fflush(stdout);
}

void term_color_set(char *color) { printf("%s", color); }
void term_color_clear(void) { printf("%s", COLOR_RESET); }

static int count_digits(int num) {
	int num_digits = 0;
	if (num == 0)
		return 1;
	while (num) {
		num /= 10;
		num_digits++;
	}
	return num_digits;
}

void window_get_size() {
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
	*offset_rows = ((term_rows - height) / 2) + 1;
	*offset_colums = ((term_colums - width) / 2);
}

void term_clear_quick(void) {
	// move cursor to top of screen and overwrite
	printf("\033[H");
}
void term_clear_full(void) {
	// move cursor to top and clear from cursor
	printf("\033[H\033[J");
}

struct termios term_normal_settings;

void term_disable_raw(void);

void term_enable_raw(void) {
	struct termios p;
	// read current parameters
	tcgetattr(STDIN_FILENO, &p);
	term_normal_settings = p;
	p.c_lflag &= ~(ECHO | ICANON);
	p.c_cc[VMIN] = 0;
	p.c_cc[VTIME] = 0;
	// set to run when exiting program
	atexit(term_disable_raw);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &p) != 0) {
		fprintf(stderr, "There was a problem setting raw terminal\n");
		exit(EXIT_FAILURE);
	}
}
void term_disable_raw(void) {
	tcsetattr(fileno(stdin), TCSANOW, &term_normal_settings);
}

typedef enum { ST_NORMAL, ST_ESC, ST_ESC_BRACKET } InState;

InputKey get_key(void) {
	char c;
	static InState state = ST_NORMAL;
	// if we read exactly one byte
	while (read(STDIN_FILENO, &c, 1) == 1) {
		switch (state) {
		case ST_NORMAL:
			if (c == 27) {
				state = ST_ESC;
			} else {
				switch (c) {
				case CONF_KEY_UP:
					return IN_UP;
				case CONF_KEY_LEFT:
					return IN_LEFT;
				case CONF_KEY_DOWN:
					return IN_DOWN;
				case CONF_KEY_RIGHT:
					return IN_RIGHT;
				case 'r':
					return IN_PLAY_AGAIN;
				case CONF_KEY_QUIT:
					return IN_QUIT;
				case '\n':
					return IN_ENTER;
				case '+':
					return IN_PLUS;
				case '-':
					return IN_MINUS;
				case 'c':
					return IN_CONFIGURE;
				default:
					return IN_OTHER;
				}
			}
			break;
		case ST_ESC:
			if (c == '[') {
				state = ST_ESC_BRACKET;
			} else {
				state = ST_NORMAL;
				return IN_OTHER;
			}
			break;
		case ST_ESC_BRACKET:
			state = ST_NORMAL;
			switch (c) {
			case 65:
				return IN_UP;
			case 66:
				return IN_DOWN;
			case 67:
				return IN_RIGHT;
			case 68:
				return IN_LEFT;
			default:
				return IN_OTHER;
			}
		}
	}
	return IN_NONE;
}

void engine_init() {
	window_get_size();
	term_enable_raw();
	term_clear_full();
}

void display_end(Board const *b, int const score, int game_over_timestamp) {
	int display_tick = 20;
	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= display_tick) {
		last_tick = now;
		term_clear_full();
		int offset_rows, offset_colums;
		term_get_offset(30, 11, &offset_rows, &offset_colums);
		printf("\033[%d;%dH", offset_rows, offset_colums);
		term_color_set(GRN);
		printf("================ csnake ================");
		term_color_clear();
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(RED);
		printf("              game over :(            ");
		term_color_clear();
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("                                    ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(MAG);
		printf("              score: %4d            ", score);
		term_color_clear();
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("                                    ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("    Press                    Press  ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("     '");
		term_color_set(CYN);
		putchar('r');
		term_color_clear();
		printf("'                      '");
		term_color_set(CYN);
		putchar('c');
		term_color_clear();
		printf("'   ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf(" to play again            to configure");
		++offset_rows;
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("       Quit with '%sq%s' at any time", CYN,
		       COLOR_RESET);
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(GRN);
		printf("========================================\n");
		term_color_clear();
	}
}

void display_welcome() {
	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= 50) {
		last_tick = now;
		term_clear_full();
		int offset_rows, offset_colums;

		term_get_offset(26, 4, &offset_rows, &offset_colums);
		printf("\033[%d;%dH", offset_rows, offset_colums);
		term_color_set(GRN);
		printf("%s", "========= csnake =========");
		term_color_clear();
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("%s", "                          ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(CYN);
		printf("%s", "   Press a key to start   ");
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(GRN);
		printf("%s", "==========================\n");
		term_color_clear();
	}

	fflush(stdout);
}

void display_configure(Board *demo, bool const is_configured_width,
		       bool const is_configured_height, float const freq,
		       int const width, int const height) {

	static int last_tick = 0;
	int now = millis();
	if (now - last_tick >= 50) {
		last_tick = now;
		term_clear_full();
		int offset_rows, offset_colums;
		term_get_offset(36, 8, &offset_rows, &offset_colums);
		printf("\033[%d;%dH", offset_rows, offset_colums);
		term_color_set(GRN);
		printf("============== csnake ==============");
		term_color_clear();
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("         Set with %s+%s/%s-%s keys\n", CYN, COLOR_RESET,
		       CYN, COLOR_RESET);
		if (is_configured_height && is_configured_width) {
			printf("\033[%d;%dH", ++offset_rows, offset_colums);
			printf("  Snake speed: %s%.1f%s tiles per second", MAG,
			       freq, COLOR_RESET);
		} else if (is_configured_width) {
			printf("\033[%d;%dH", ++offset_rows, offset_colums);
			printf("       Board height: %s%d%s tiles", MAG, height,
			       COLOR_RESET);
		} else {
			printf("\033[%d;%dH", ++offset_rows, offset_colums);
			printf("       Board width: %s%d%s tiles", MAG, width,
			       COLOR_RESET);
		}
		++offset_rows;
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("         Press %senter%s to set", CYN, COLOR_RESET);
		++offset_rows;
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		printf("     Quit with '%sq%s' at any time", CYN, COLOR_RESET);
		printf("\033[%d;%dH", ++offset_rows, offset_colums);
		term_color_set(GRN);
		printf("====================================\n");
		term_color_clear();
	}
}
