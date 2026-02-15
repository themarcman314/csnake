typedef struct Board Board;


Board *board_create(int width, int height);
void board_print_info(const Board *b);
int board_get_width(const Board *b);
int board_get_height(const Board *b);
char board_get_square(const Board *b, const int x, const int y);
