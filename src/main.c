#include "grid.h"
#include "draw.h"
#include "snake.h"
#include <stdio.h>
#include <stdbool.h>

bool time_passed(const unsigned ms);
void snake_move_head(struct snake *s, const unsigned direction);

int main(void)
{
    struct coordinates c = {0, 0};
    struct snake s;
    init_snake(&s);


    while(1)
    {
        if(time_passed(500))
        {
            map_draw(&c, &s);
            snake_move_head(&s, ORIENTATION_EAST);
        }

    }



    return 0;
}
