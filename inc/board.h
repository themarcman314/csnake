typedef struct Board Board;

Board *board_create(const int width, const int height);

void board_print_info(const Board *b);

int board_get_width(const Board *b);

int board_get_height(const Board *b);

char board_get_square(const Board *b, const int x, const int y);

void board_set_square(Board *b, const int x, const int y, const char c);
