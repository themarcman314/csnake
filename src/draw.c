#include <stdio.h>
#include "grid.h"
#include "draw.h"

void map_draw(struct coordinates *c)
{
    for(c->y = 0; c->y < GRID_SIZE_Y; c->y++)
    {
        for(c->x = 0; c->x < GRID_SIZE_X; c->x++)
        {
            switch (c->x)
            {
            case 0:
                switch (c->y)
                {
                case 0:
                    printf("\u250F");
                    break;
                case GRID_SIZE_Y - 1:
                    printf("\u2517");
                    break;
                default:
                    printf("\u2503");
                    break;
                }
                break;

            case GRID_SIZE_X - 1:
                switch (c->y)
                {
                case 0:
                    printf("\u2513");
                    break;
                case GRID_SIZE_Y - 1:
                    printf("\u251B");
                    break;
                default:
                    printf("\u2503");
                    break;
                }
                break;
            default:
                switch (c->y)
                {
                case 0:
                    printf("\u2501");
                    break;
                case GRID_SIZE_Y - 1:
                    printf("\u2501");
                    break;
                default:
                    block_draw(c);
                    break;
                }
            }
        }
        printf("\n");
    }
}

void block_draw(struct coordinates *c)
{
    printf(" ");
}