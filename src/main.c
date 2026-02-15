#include "board.h"
#include "engine.h"
#include <bits/time.h>
#include <stdio.h>
#include <time.h>

typedef struct {
	int state;
	int score;
	int tick_speed;
	Board *b;
} Game;

void game_init(Game *g);
void game_run(Game *g);
long long millis(void);

int main(void) {
	Game g;
	game_init(&g);
	game_run(&g);
}

void game_init(Game *g) {
	g->tick_speed = 500;
	g->b = board_create(50, 20);
	for (int y = 0; y < board_get_height(g->b); y++) {
		for (int x = 0; x < board_get_width(g->b); x++) {
			board_set_square(g->b, x, y, '-');
		}
	}
	board_print_info(g->b);
}

void game_run(Game *g) {
	int last = millis();
	while (1) {
		int now = millis();
		if (now - last >= g->tick_speed) {
			last += g->tick_speed;
			static int a;
			board_draw(g->b);
		}
	}

}

long long millis(void) {
	int ms;
#ifdef _WIN32
#error "This program does not support windows :("
#else
	//printf("this is posix system");
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ms = ts.tv_sec *
		 1000LL + // ensure 64 bit arithmetic to avoid multiply overflow
	     ts.tv_nsec / 1000000;
#endif
	return ms;
}
