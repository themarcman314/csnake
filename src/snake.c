#include "snake.h"

void init_snake(struct snake *s)
{
    // set initial position at the middle
    s->head.x = GRID_SIZE_X/2; 
    s->head.y = GRID_SIZE_Y/2; 
}