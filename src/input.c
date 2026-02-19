#include <stdlib.h>
#ifdef _WIN32
#else
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

TermInputKey term_get_key(void) {
	char c;
	TermInputKey key = IN_NONE;
	// if we read exactly one byte
	if (read(STDIN_FILENO, &c, 1) == 1) {
		switch (c) {
		case 'w':
			key = IN_UP;
			break;
		case 'a':
			key = IN_LEFT;
			break;
		case 's':
			key = IN_DOWN;
			break;
		case 'd':
			key = IN_RIGHT;
			break;

		default:
			break;
		}
	}
	return key;
}

#endif
