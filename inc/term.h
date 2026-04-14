#ifndef TERM
#define TERM
#include "board.h"

void term_enable_raw(void);
void term_print_size(void);
void term_clear_full(void);
void term_disable_raw(void);
void term_get_offset(const int width, const int height, int *offset_row,
		     int *offset_colums);
void term_color_set(char *color);
void term_color_clear(void);
void term_clear_quick(void);
void term_print_spaces(int const num);
void term_print_newlines(int const num);
#endif
