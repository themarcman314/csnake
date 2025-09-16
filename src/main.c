#include "grid.h"
#include "draw.h"
#include "snake.h"
#include <stdio.h>
#include <stdbool.h>
#include "input.h"

bool time_passed(const unsigned ms);
void snake_move_head(struct snake *s, const unsigned direction);

// map key returns to snake orientation function calls
typedef void (*snake_move_head_ptr)(struct snake *s);

void snake_move_head_N(struct snake *s);
void snake_move_head_S(struct snake *s);
void snake_move_head_E(struct snake *s);
void snake_move_head_W(struct snake *s);

snake_move_head_ptr snake_move_funcs[] = 
{
   [KEY_UP] = snake_move_head_N,
   [KEY_DOWN] = snake_move_head_S,
   [KEY_LEFT] = snake_move_head_W,
   [KEY_RIGHT] = snake_move_head_E
};

int main(void)
{
    struct coordinates c = {0, 0};
    struct snake s;
    init_snake(&s);
    init_termios();

    while(1)
    {
        static int key_read = -1;
        int key_scanned;
        key_scanned = read_arrow_key_in();
        if(key_scanned >= 0)
            key_read = key_scanned;
        if(time_passed(100))
        {
            map_draw(&c, &s);
            if(key_read>=0)
                snake_move_funcs[key_read](&s);
        }
    }
    return 0;
}
