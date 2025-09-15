// snake should move every tick.
// tick duration should be configurable
// use multiple of ms?

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "snake.h"
#include "grid.h"

#define HORIZONTAL_MOVE_SPEED 3

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
void snake_move_head(struct snake *s, const unsigned direction)
{
    switch (direction)
    {
    case ORIENTATION_NORTH:
        s->orientation = ORIENTATION_NORTH;
        s->head.y--;
        if (s->head.y == 0)
        {
            s->head.y = GRID_SIZE_Y - 2;
        }
        break;
    case ORIENTATION_SOUTH:
        s->orientation = ORIENTATION_SOUTH;
        s->head.y++;
        if (s->head.y == GRID_SIZE_Y - 1)
        {
            s->head.y = 1;
        }
        break;
    case ORIENTATION_EAST:
        s->orientation = ORIENTATION_EAST;
        s->head.x = s->head.x + HORIZONTAL_MOVE_SPEED;
        if (s->head.x >= GRID_SIZE_X-1)
        {
            s->head.x = 1;
        }
        break;
    case ORIENTATION_WEST:
        s->orientation = ORIENTATION_WEST;
        s->head.x = s->head.x - HORIZONTAL_MOVE_SPEED;
        if (s->head.x <= 0)
        {
            s->head.x = GRID_SIZE_X - 2;
        }
        break;
    
    default:
        break;
    }
}