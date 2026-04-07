#include "raylib.h"
#include <stdio.h>
#include "game.h"
#include <stdlib.h>

void DrawBoard(int const board_border_size, int const board_size, int const grid_thickness,
	       int const delta, Color c);

int main(void) {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	printf("hello world!\n");
	InitWindow(50, 50, "my hello world window");
	SetTargetFPS(60);
	int const screenWidth = GetMonitorWidth(GetCurrentMonitor());
	int const screenHeight = GetMonitorHeight(GetCurrentMonitor());
	printf("width: %d\nheight %d\n", screenWidth, screenHeight);
	int const win_border_size = 50;
	int const window_width = screenWidth - win_border_size;
	int const window_height = screenHeight - win_border_size;
	SetWindowSize(window_width, window_height);
	SetWindowPosition(win_border_size / 2, win_border_size / 2);
	int const board_border_size = 20;
	int const board_size = 10;

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		int const delta = window_height / board_size;
		DrawBoard(board_border_size, board_size, 5, delta, LIGHTGRAY);

		EndDrawing();
	}
	CloseWindow();
	game_fsm_run();
	return EXIT_SUCCESS;
}


void DrawBoard(int const board_border_size, int const board_size, int const grid_thickness,
	       int const delta, Color c) {
	// columns
	for (int i = 0; i < board_size; i++) {
		DrawRectangle(board_border_size + i * delta, board_border_size,
			      grid_thickness, (board_size - 1) * delta + grid_thickness,
			      c);
	}
	// lines
	for (int i = 0; i < board_size; i++) {
		DrawRectangle(board_border_size, board_border_size + i * delta,
			      (board_size - 1) * delta, grid_thickness, c);
	}
}
