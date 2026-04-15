#include "board.h"
#include "conf.h"
#include "engine.h"
#include "input.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

bool is_azerty = false;

struct DrawingParameters {
	int const delta;
	int const board_border_size;
	int const board_wall_thickness;
	Color color;
};

DrawingParameters p = {
    .delta = 25, .board_border_size = 20, .board_wall_thickness = 5};

void set_keyboard_type() {
	if (*GetKeyName(KEY_A) == 'q') {
		is_azerty = true;
		// if (is_azerty) {
		//	char title[] = "is azerty";
		//	int font_size = 35;
		//	DrawText(title, 100 - MeasureText(title, font_size) / 2,
		//		 500, font_size, GREEN);
		// }
	}
}

InputKey get_key(void) {

	int const key = GetKeyPressed();
	switch (key) {
		if (is_azerty) {
		case KEY_Z:
			return IN_UP;
			break;
		case KEY_Q:
			return IN_LEFT;
			break;
		} else {
		case KEY_W:
			return IN_UP;
			break;
		case KEY_A:
			return IN_LEFT;
			break;
		}
	case KEY_D:
		return IN_RIGHT;
		break;
	case KEY_S:
		return IN_DOWN;
		break;

	case KEY_UP:
		return IN_UP;
		break;
	case KEY_DOWN:
		return IN_DOWN;
		break;
	case KEY_LEFT:
		return IN_LEFT;
		break;
	case KEY_RIGHT:
		return IN_RIGHT;
		break;
	case KEY_ENTER:
		return IN_ENTER;
		break;
	case KEY_R:
		return IN_PLAY_AGAIN;
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

void grid_draw(int const board_size_x, int const board_size_y,
	       int const board_border_size, int const grid_thickness,
	       int const delta, Color c) {
	// columns
	for (int i = 0; i <= board_size_x; i++) {
		DrawRectangle(board_border_size + i * delta, board_border_size,
			      grid_thickness,
			      board_size_y * delta + grid_thickness, c);
	}
	// lines
	for (int i = 0; i <= board_size_y; i++) {
		DrawRectangle(board_border_size, board_border_size + i * delta,
			      board_size_x * delta, grid_thickness, c);
	}
}

void board_draw(Board const *b, int const score) {
	ClearBackground(RAYWHITE);

	grid_draw(b->width, b->height, p.board_border_size,
		  p.board_wall_thickness, p.delta, LIGHTGRAY);

	for (int y = 0; y < b->height; y++) {
		for (int x = 0; x < b->width; x++) {
			const char c = board_get_square(b, x, y);
			if (c == SNAKE_HEAD_CHAR || c == SNAKE_BODY_CHAR) {
				draw_square(&p, x, y, GREEN);
			}
			if (c == FOOD_CHAR) {
				draw_square(&p, x, y, ORANGE);
			}
		}

		// #define SNAKE_HEAD_CHAR '@'
		// #define SNAKE_BODY_CHAR 'o'
		// #define FOOD_CHAR '*'
	}
}

void draw_square(DrawingParameters const *p, int const x, int const y,
		 Color c) {
	int pixel_start_x =
	    p->board_border_size + p->board_wall_thickness + p->delta * x;
	int pixel_start_y =
	    p->board_border_size + p->board_wall_thickness + p->delta * y;
	DrawRectangle(pixel_start_x, pixel_start_y,
		      p->delta - p->board_wall_thickness,
		      p->delta - p->board_wall_thickness, c);
}

void display_welcome() {
	ClearBackground(RAYWHITE);
	set_keyboard_type();

	int const width = GetScreenWidth();
	int const height = GetScreenHeight();
	char title[] = "csnake";
	int const font_size = 35;
	DrawText(title, width / 2 - MeasureText(title, font_size) / 2,
		 height / 4, font_size, GREEN);
}

void display_end(Board const *b, int const score, int game_over_timestamp) {
	int x, y;
	snake_get_head_position(b->s, &x, &y);
	board_draw(b, score);
	board_draw_collision(b, x, y);
	int now = millis();
	if (now - game_over_timestamp < 1000) {
		return;
	}
	int const screen_width = GetScreenWidth();
	int const screen_height = GetScreenHeight();
	int font_size_big = 35;
	int font_size_small = 20;
	char text[] = "game over :(";
	char restart_text[] = "press 'r' to play again";
	char score_text[20];
	sprintf(score_text, "score: %d", score);
	// ClearBackground(RAYWHITE);
	DrawText(text, screen_width / 2 - MeasureText(text, font_size_big) / 2,
		 screen_height / 4, font_size_big, RED);
	DrawText(score_text,
		 screen_width / 2 - MeasureText(score_text, font_size_big) / 2,
		 screen_height / 4 + 50, font_size_big, MAROON);
	DrawText(restart_text,
		 screen_width / 4 -
		     MeasureText(restart_text, font_size_small) / 2,
		 3 * screen_height / 4, font_size_small, BLACK);
}

void display_configure(bool const is_configured_width,
		       bool const is_configured_height, float const freq,
		       int const width, int const height) {

	ClearBackground(RAYWHITE);

	int const screen_width = GetScreenWidth();
	int const screen_height = GetScreenHeight();
	char title[] = "configuring...";
	int const font_size = 35;
	DrawText(title, screen_width / 2 - MeasureText(title, font_size) / 2,
		 screen_height / 4, font_size, GREEN);
}

void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y) {
	draw_square(&p, board_x, board_y, RED);
}
void window_get_size() {}

void window_periodic_start() { BeginDrawing(); }
void window_periodic_end() { EndDrawing(); }
