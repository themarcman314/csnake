#include <stdio.h>
#include "board.h"
#include "engine.h"


int main(void) {
	Board *b;
	b = board_create(50, 20);
	for(int y = 0; y < board_get_height(b); y++) {
		for(int x = 0; x < board_get_width(b); x++) {
			board_set_square(b, x, y, '-');
		}
	}
	board_print_info(b);
	board_draw(b);
}
