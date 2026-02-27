#include <stdlib.h>
#ifdef _WIN32
#else
#include "conf.h"
#include "input.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

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

TermInputKey term_get_key(void) {
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
				case KEY_UP:
					return IN_UP;
				case KEY_LEFT:
					return IN_LEFT;
				case KEY_DOWN:
					return IN_DOWN;
				case KEY_RIGHT:
					return IN_RIGHT;
				case 'r':
					return IN_PLAY_AGAIN;
				case KEY_QUIT:
					return IN_QUIT;
				case '\n':
					return IN_ENTER;
				case '+':
					return IN_PLUS;
				case '-':
					return IN_MINUS;
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
			default: return IN_OTHER;
			}
		}
	}
	return IN_NONE;
}

#endif
