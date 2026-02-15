#include <stdio.h>
#include "board.h"
#include "engine.h"


int main(void) {
	Board *b;
	b = board_create(5, 5);
	board_print_info(b);
	board_draw(b);
}
