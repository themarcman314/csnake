#include "board.h"
#include "engine.h"
#include "input.h"
#include <stdlib.h>

#ifdef _WIN32
#error "This program does not support windows :("
#else
#include <time.h>
#endif

typedef struct {
	int state;
	int score;
	int tick_speed;
	Board *b;
} Game;

void game_init(Game *g);
void game_end(Game *g);
void game_run(Game *g);
int millis(void);

int main(void) {
	Game g;
	game_init(&g);
	game_run(&g);

	return EXIT_SUCCESS;
}

void game_init(Game *g) {
	term_enable_raw();
	g->tick_speed = 500;
	g->b = board_create(50, 20);
}

void game_end(Game *g) {
	board_destroy(g->b);
}

void game_run(Game *g) {
	int last = millis();
	while (1) {
		snake_head_set_direction(g->b);
		int now = millis();
		if (now - last >= g->tick_speed) {
			last += g->tick_speed;
			board_update(g->b);
			if(board_check_collisions(g->b)) break;
			board_draw(g->b);
		}
	}
}

int millis(void) {
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
