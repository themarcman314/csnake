#include "snake.h"
#include "grid.h"
#include "food.h"


void map_draw(struct coordinates *c, const struct snake *s, const struct food *f);
void block_draw(const struct coordinates *cur_pos, const struct snake *s, const struct food *f);