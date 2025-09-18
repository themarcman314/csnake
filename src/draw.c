#include <stdio.h>
#include "grid.h"
#include "draw.h"
#include "snake.h"


void food_init(struct food *food);
void food_spawn(struct food *food);

void map_draw(struct coordinates *c, const struct snake *s, const struct food *f)
{
    // clear screen
    printf("\e[1;1H\e[2J");
    printf("food:\n x: %d\n y: %d\n", f->c_f.x, f->c_f.y);

    for(c->y = 0; c->y <= GRID_SIZE_Y; c->y++)
    {
        for(c->x = 0; c->x <= GRID_SIZE_X; c->x++)
        {
            switch (c->x)
            {
            case 0:
                switch (c->y)
                {
                case 0:
                    printf("\u250F");
                    break;
                case GRID_SIZE_Y:
                    printf("\u2517");
                    break;
                default:
                    printf("\u2503");
                    break;
                }
                break;
            case GRID_SIZE_X:
                switch (c->y)
                {
                case 0:
                    printf("\u2513");
                    break;
                case GRID_SIZE_Y:
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
                case GRID_SIZE_Y:
                    printf("\u2501");
                    break;
                default:
                    block_draw(c, s, f);
                    
                    break;
                }
            }
        }
        printf("\n");
    }
}

void block_draw(const struct coordinates *cur_pos, const struct snake *s, const struct food *f)
{

    if(s->head.x == cur_pos->x && s->head.y == cur_pos->y)
    {
        switch (s->orientation)
        {
        case ORIENTATION_NORTH:
            printf("\u25B2");
            break;
        case ORIENTATION_EAST:
            printf("\u25B6");
            break;
        case ORIENTATION_SOUTH:
            printf("\u25BC");
            break;
        case ORIENTATION_WEST:
            printf("\u25C0");
            break;
        
        default:
            break;
        }

    }
    else if (cur_pos->x == f->c_f.x && cur_pos->y == f->c_f.y)
    {
        printf("\u25cf");
    }
    else printf(" ");
}