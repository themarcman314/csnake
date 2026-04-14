#include "board.h"
#include "engine.h"
#include "input.h"
#include "raylib.h"
#include <stdio.h>

InputKey get_key(void) {

	int const key = GetKeyPressed();
	switch (key) {
	case KEY_UP:
		return IN_UP;
		break;
	case KEY_ENTER:
		return IN_ENTER;
		break;
	default:
		break;
	}

	if (WindowShouldClose())
		return IN_QUIT;
	return IN_NONE;
}
void engine_init() {
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
}

void grid_draw(int const board_border_size, int const board_size,
	       int const grid_thickness, int const delta, Color c) {
	// columns
	for (int i = 0; i < board_size; i++) {
		DrawRectangle(board_border_size + i * delta, board_border_size,
			      grid_thickness,
			      (board_size - 1) * delta + grid_thickness, c);
	}
	// lines
	for (int i = 0; i < board_size; i++) {
		DrawRectangle(board_border_size, board_border_size + i * delta,
			      (board_size - 1) * delta, grid_thickness, c);
	}
}

void board_draw(Board const *b, int const score) {

	ClearBackground(RAYWHITE);

	int const delta = 10;
	int const board_border_size = 20;
	int const board_size = 10;

	grid_draw(board_border_size, board_size, 5, delta, LIGHTGRAY);

	EndDrawing();
}

void display_welcome() {
	ClearBackground(RAYWHITE);

	int const width = GetScreenWidth();
	int const height = GetScreenHeight();
	char title[] = "csnake";
	int const font_size = 35;
	DrawText(title, width / 2 - MeasureText(title, font_size) / 2,
		 height / 4, font_size, GREEN);
}

void display_end(int score) {}

void display_configure(bool const is_configured_width,
		       bool const is_configured_height, float const freq,
		       int const width, int const height) {}

void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y) {}
void window_get_size() {}

void window_periodic_start() { BeginDrawing(); }
void window_periodic_end() { EndDrawing(); }
