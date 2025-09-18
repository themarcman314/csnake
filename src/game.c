#include "grid.h"
#include "draw.h"
#include "snake.h"
#include <stdio.h>
#include <stdbool.h>
#include "input.h"
#include "food.h"
#include "player.h"

void food_init(struct food *f);
bool time_passed(const unsigned ms);
void food_spawn(struct food *food);

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


struct coordinates c = {0, 0};
struct snake s = {0};
struct food f = {0};
struct player p = {0};

void game_init(void)
{
    init_snake(&s);
    init_termios();
    food_init(&f);
    food_spawn(&f);
}

void game_run(void)
{
    static int key_read = -1;
    int key_scanned;
    key_scanned = read_arrow_key_in();
    if(key_scanned >= 0)
        key_read = key_scanned;
    if(time_passed(200))
    {
        if(key_read>=0)
            snake_move_funcs[key_read](&s);
        if(s.head.x == f.c_f.x && s.head.y == f.c_f.y) // if snake eats food
        {
            food_spawn(&f); // spawn more
            p.score++;
            s.length++;
        }
        map_draw(&c, &s, &f);
    }
}