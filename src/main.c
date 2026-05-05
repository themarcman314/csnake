#include "game.h"
#include "raylib.h"
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static Game *g;
void update_frame_wrapper(void);

int main(void) {
	g = game_create();
#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(update_frame_wrapper, 0, 1);
#else
	game_fsm_run(g);

#endif
	game_clean(g);
	CloseWindow();
	return EXIT_SUCCESS;
}

void update_frame_wrapper(void) { UpdateDrawFrame(g); }
