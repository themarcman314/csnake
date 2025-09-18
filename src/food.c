
// spawn food randomly on grid
// food has to fall on even coordinates horizontally

#include <time.h>
#include <stdlib.h>
#include "food.h"

void food_init(struct food *food)
{
    srand( time( NULL ) );

    food->min = 2;
    food->max = GRID_SIZE_X-2;

    // Ensure min is even
    if (food->min % 2 != 0) food->min++;

    // Number of even values in range
    food->even_count = ((food->max - food->min) / 2) + 1;

}

void food_spawn(struct food *food)
{

    // Pick random even
    food->c_f.x = food->min + 2 * (rand() % food->even_count);

    food->c_f.y = food->min + rand()%food->max;
}