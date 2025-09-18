#include "grid.h"

enum snake_orientation {
  ORIENTATION_NORTH,
  ORIENTATION_SOUTH,
  ORIENTATION_WEST,
  ORIENTATION_EAST
}; 

struct snake
{
    struct coordinates head;
    int orientation;
    // TODO: add other elements
    // recursive?
    // linked list since body gets longer??
};

void init_snake(struct snake *s);