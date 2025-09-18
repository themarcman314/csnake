#include "game.h"

int main(void) 
{
    game_init();
    while(1)
    {
        game_run();
    }
    return 0;
}
