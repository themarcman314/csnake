// snake should move every tick.
// tick duration should be configurable
// use multiple of ms?

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "snake.h"
#include "grid.h"

#define HORIZONTAL_MOVE_SPEED 2

bool time_passed(const unsigned ms)
{
    static clock_t start;
    clock_t now = clock();
    unsigned diff = ((now - start) *1000 / CLOCKS_PER_SEC);
    if( diff > ms)
    {
        start = clock();
        return true;
    }
    return false;
}

// add snake turn head function? to make it seem more reactive?

void snake_move_head_N(struct snake *s)
{
    s->orientation = ORIENTATION_NORTH;
    s->head.y--;
    if (s->head.y == 0)
    {
        s->head.y = GRID_SIZE_Y - 2;
    }
}

void snake_move_head_S(struct snake *s)
{
    s->orientation = ORIENTATION_SOUTH;
    s->head.y++;
    if (s->head.y == GRID_SIZE_Y - 1)
    {
        s->head.y = 1;
    }
}
void snake_move_head_E(struct snake *s)
{
    s->orientation = ORIENTATION_EAST;
    s->head.x = s->head.x + HORIZONTAL_MOVE_SPEED;
    if (s->head.x >= GRID_SIZE_X-1)
    {
        s->head.x = 2;
    }
}
void snake_move_head_W(struct snake *s)
{
    s->orientation = ORIENTATION_WEST;
    s->head.x = s->head.x - HORIZONTAL_MOVE_SPEED;
    if (s->head.x <= 0)
    {
        s->head.x = GRID_SIZE_X - 2;
    }
}