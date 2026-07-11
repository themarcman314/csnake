#include "board.h"
#include "conf.h"
#include "engine.h"
#include "game.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKGROUND_COLOR CLITERAL(Color){31, 31, 31, 255}
#define TEXT_COLOR WHITE
#define GRID_COLOR DARKGRAY

bool is_azerty = false;

struct DrawingParameters {
	bool draw_fps;
	int screen_width;
	int screen_height;
	int const delta;
	int const board_wall_thickness;
	int start_x;
	int start_y;
	int const font_size_big;
	int const font_size_small;
};

DrawingParameters p = {.draw_fps = true,
		       .delta = 25,
		       .board_wall_thickness = 5,
		       .font_size_big = 35,
		       .font_size_small = 20};

void set_keyboard_type() {
#ifndef PLATFORM_WEB
	if (*GetKeyName(KEY_A) == 'q') {
		is_azerty = true;
		// if (is_azerty) {
		//	char title[] = "is azerty";
		//	int font_size = 35;
		//	DrawText(title, 100 - MeasureText(title, font_size) / 2,
		//		 500, font_size, GREEN);
		// }
	}
#endif
}

void engine_init() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "csnake");
	SetTargetFPS(60);
	int const screenWidth = GetMonitorWidth(GetCurrentMonitor());
	int const screenHeight = GetMonitorHeight(GetCurrentMonitor());
	int const win_border_size = 50;
	int const window_width = screenWidth - win_border_size;
	int const window_height = screenHeight - win_border_size;
	SetWindowSize(window_width, window_height);
	SetWindowPosition(win_border_size / 2, win_border_size / 2);
}

void grid_draw(int const board_size_x, int const board_size_y,
	       int const start_x, int const start_y, int const grid_thickness,
	       int const delta, Color c) {
	// columns
	for (int i = 0; i <= board_size_x; i++) {
		DrawRectangle(start_x + i * delta, start_y, grid_thickness,
			      board_size_y * delta + grid_thickness, c);
	}
	// lines
	for (int i = 0; i <= board_size_y; i++) {
		DrawRectangle(start_x, start_y + i * delta,
			      board_size_x * delta, grid_thickness, c);
	}
}

void board_draw(Board const *b, int score, bool is_draw_game_over,
		bool show_score) {
	ClearBackground(BACKGROUND_COLOR);

	set_start_coords_grid(b->width, b->height);

	grid_draw(b->width, b->height, p.start_x, p.start_y,
		  p.board_wall_thickness, p.delta, GRID_COLOR);

	for (int y = 0; y < b->height; y++) {
		for (int x = 0; x < b->width; x++) {
			const char c = board_get_square(b, x, y);
			if (c == FOOD_CHAR) {
				draw_square(&p, x, y, ORANGE);
			} else if (c == SNAKE_BODY_CHAR ||
				   (c == SNAKE_HEAD_CHAR && !is_draw_game_over))
				draw_square(&p, x, y, GREEN);
			if (show_score) {
				char score_text[20] = "";
				sprintf(score_text, "score: %d", score);
				DrawText(score_text,
					 p.screen_width - 50 -
					     MeasureText(score_text,
							 p.font_size_big),
					 50, p.font_size_big, BLUE);
			}
		}
	}
}

void draw_square(DrawingParameters const *p, int const x, int const y,
		 Color c) {
	int pixel_start_x = p->start_x + p->board_wall_thickness + p->delta * x;
	int pixel_start_y = p->start_y + p->board_wall_thickness + p->delta * y;
	DrawRectangle(pixel_start_x, pixel_start_y,
		      p->delta - p->board_wall_thickness,
		      p->delta - p->board_wall_thickness, c);
}

void display_welcome() {
	ClearBackground(BACKGROUND_COLOR);
	set_keyboard_type();

	int const width = GetScreenWidth();
	int const height = GetScreenHeight();
	int const title_height = height / 4;
	char title[] = "csnake";
	int const font_size = p.font_size_big;
	DrawText(title, width / 2 - MeasureText(title, p.font_size_big) / 2,
		 title_height, p.font_size_big, GREEN);
	char press_key[] = "press any key to start...";
	DrawText(press_key,
		 width / 2 - MeasureText(press_key, p.font_size_small) / 2,
		 title_height + 2 * p.font_size_big, p.font_size_small,
		 TEXT_COLOR);
	char music_credit_yt_link[] =
	    "Song credit goes to: youtube.com/@knox_limited";
	DrawText(music_credit_yt_link,
		 width / 2 -
		     MeasureText(music_credit_yt_link, p.font_size_small) / 2,
		 height - p.font_size_big, p.font_size_small, TEXT_COLOR);
}

void display_end(Board const *b, int const score, int game_over_timestamp) {
	ClearBackground(BACKGROUND_COLOR);
	int x, y;
	snake_get_head_position(b->s, &x, &y);
	board_draw(b, score, true, false);
	board_draw_collision(b, x, y);
	int now = millis();
	if (now - game_over_timestamp < 1000) {
		DrawText(".", -10, -10, 1, WHITE);
		return;
	}
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
		      Fade(BLACK, 0.7f));
	int const screen_width = GetScreenWidth();
	int const screen_height = GetScreenHeight();
	char text[] = "game over :(";
	char restart_text[] = "press 'r' to play again";
	char high_score_text[] = "press 'h' to view high scores";
	char score_text[20];
	sprintf(score_text, "score: %d", score);
	// board_draw(b, score, false);
	DrawText(text,
		 screen_width / 2 - MeasureText(text, p.font_size_big) / 2,
		 screen_height / 5, p.font_size_big, RED);
	DrawText(score_text,
		 screen_width / 2 -
		     MeasureText(score_text, p.font_size_big) / 2,
		 screen_height / 5 + 50, p.font_size_big, BLUE);
	DrawText(restart_text,
		 screen_width / 2 -
		     MeasureText(restart_text, p.font_size_small) / 2,
		 3 * screen_height / 4, p.font_size_small, TEXT_COLOR);
	// DrawText(high_score_text,
	//	 3 * screen_width / 4 -
	//	     MeasureText(high_score_text,
	// p.font_size_small) /
	// 2, 	 3 * screen_height / 4, p.font_size_small,
	// BLACK);
}

void set_start_coords_grid(int grid_width, int grid_height) {
	p.start_x =
	    (p.screen_width - (p.board_wall_thickness + p.delta * grid_width)) /
	    2;
	p.start_y = (p.screen_height -
		     (p.board_wall_thickness + p.delta * grid_height)) /
		    2;
}

void display_menu_conf(DisplayConfigureInfo const info) {
	ClearBackground(BACKGROUND_COLOR);
	int const border_fraction_screen_width = 15;
	int const border_fraction_screen_height = 15;
	int const rectangle_thickness_lines = 2;
	int rectangle_height = p.screen_height / 20;
	int rectangle_width = (border_fraction_screen_width - 2) *
			      p.screen_width / border_fraction_screen_width;
	int rectangle_height_spacing =
	    p.screen_height / (info.element_count + 1);
	int rectangle_x = p.screen_width / border_fraction_screen_width;
	int rectangle_y_base = p.screen_height / border_fraction_screen_height;
	int rectangle_fill_offset = 5;

	char value[4][20];
	memset(value, 0, sizeof(value));
	sprintf(value[0], "%d", info.width);
	sprintf(value[1], "%d", info.height);
	sprintf(value[2], "%.2f", info.freq);
	sprintf(value[3], "%s", info.board_wrapping ? "enabled" : "disabled");

	for (int i = 0; i < info.element_count; i++) {
		const char *labels[] = {"Board width", "Board height",
					"Snake speed", "Board wrapping"};
		// Rectangle r = {rectangle_x,
		//	       i * rectangle_height_spacing +
		// rectangle_y_base, 	       rectangle_width,
		// rectangle_height};
		DrawRectangleLinesEx(info.elements[i].bounds,
				     rectangle_thickness_lines, GRAY);
		if (info.state_select == i)
			DrawRectangle(
			    rectangle_x + rectangle_fill_offset,
			    i * rectangle_height_spacing + rectangle_y_base +
				rectangle_fill_offset,
			    rectangle_width - 2 * rectangle_fill_offset,
			    rectangle_height - 2 * rectangle_fill_offset,
			    GREEN);
		DrawText(labels[i], rectangle_x + rectangle_fill_offset,
			 i * rectangle_height_spacing + rectangle_y_base,
			 rectangle_height - rectangle_fill_offset, TEXT_COLOR);

		DrawText(value[i],
			 rectangle_x + rectangle_width -
			     MeasureText(value[i], rectangle_height -
						       rectangle_fill_offset) -
			     rectangle_fill_offset,
			 i * rectangle_height_spacing + rectangle_y_base,
			 rectangle_height - rectangle_fill_offset, BLUE);
	}
	char play_button[] = "Play";
	int button_width =
	    MeasureText(play_button, rectangle_height - rectangle_fill_offset);
	Rectangle r = {
	    p.screen_width - rectangle_x - button_width -
		2 * rectangle_fill_offset,
	    info.element_count * rectangle_height_spacing + rectangle_y_base,
	    button_width + rectangle_fill_offset * 2, rectangle_height};
	DrawRectangleLinesEx(r, rectangle_thickness_lines, GRAY);
	if (info.state_select == STATE_CONFIGURE_SELECTED_PLAY)
		DrawRectangle(p.screen_width - rectangle_x - button_width -
				  rectangle_fill_offset,
			      info.element_count * rectangle_height_spacing +
				  rectangle_y_base + rectangle_fill_offset,
			      button_width,
			      rectangle_height - rectangle_fill_offset * 2,
			      GREEN);
	DrawText(
	    play_button,
	    p.screen_width - rectangle_x - button_width - rectangle_fill_offset,
	    info.element_count * rectangle_height_spacing + rectangle_y_base,
	    rectangle_height - rectangle_fill_offset, TEXT_COLOR);
}
void display_name_conf(DisplayConfigureInfo const info) {
	ClearBackground(BACKGROUND_COLOR);
	char title_name[] = "Enter your name:";
	DrawText(title_name,
		 p.screen_width / 2 -
		     MeasureText(title_name, p.font_size_big) / 2,
		 p.screen_height / 4, p.font_size_big, TEXT_COLOR);
	Rectangle textBox = {p.screen_width / 2.0f - 100,
			     p.screen_height / 4.f + 50, 225, 50};
	DrawRectangleRec(textBox, LIGHTGRAY);
	DrawText(info.name, (int)textBox.x + 5, (int)textBox.y + 8,
		 p.font_size_big, MAROON);
}

void display_width_conf(DisplayConfigureInfo const info) {
	ClearBackground(BACKGROUND_COLOR);
	display_menu_conf(info);
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
		      Fade(BLACK, 0.7f));
	set_start_coords_grid(info.width, info.height);
	grid_draw(info.width, info.height, p.start_x, p.start_y,
		  p.board_wall_thickness, p.delta, GRID_COLOR);
	char title_width[] = "Set board width:";
	DrawText(title_width,
		 p.screen_width / 2 -
		     MeasureText(title_width, p.font_size_big) / 2,
		 p.screen_height / 4, p.font_size_big, WHITE);
	char width_number_string[5];
	char width_string[] = " tiles wide";
	sprintf(width_number_string, "%d", info.width);
	int const width_number_string_len =
	    MeasureText(width_number_string, p.font_size_big);
	int const width_string_len = MeasureText(width_string, p.font_size_big);
	DrawText(width_number_string,
		 p.screen_width / 2 -
		     (width_number_string_len + width_string_len) / 2,
		 p.font_size_big + p.screen_height / 4, p.font_size_big, BLUE);
	DrawText(width_string,
		 p.screen_width / 2 -
		     (width_number_string_len + width_string_len) / 2 +
		     width_number_string_len,
		 p.font_size_big + p.screen_height / 4, p.font_size_big,
		 MAROON);
	int btn_width = 100, btn_height = 50;
	int inner_spacing = 50;
	UIElement decrease =
	    CreateButton(p.screen_width / 2 - (btn_width + inner_spacing),
			 p.screen_height / 4 + 2 * p.font_size_big, btn_width,
			 btn_height, "-");
	UIElement increase =
	    CreateButton(p.screen_width / 2 + inner_spacing,
			 p.screen_height / 4 + 2 * p.font_size_big, btn_width,
			 btn_height, "+");
	DrawRectangleLinesEx(decrease.bounds,
			     decrease.rectangle_thickness_lines, GRAY);
	DrawRectangleLinesEx(increase.bounds,
			     increase.rectangle_thickness_lines, GRAY);

	Vector2 current_mouse_pos = GetMousePosition();
	decrease.is_hovered =
	    CheckCollisionPointRec(current_mouse_pos, decrease.bounds);
	increase.is_hovered =
	    CheckCollisionPointRec(current_mouse_pos, increase.bounds);
	if (decrease.is_hovered)
		DrawRectangleRec(decrease.highlighted_portion, GREEN);
	if (increase.is_hovered)
		DrawRectangleRec(increase.highlighted_portion, GREEN);
}

void display_height_conf(DisplayConfigureInfo const info) {
	ClearBackground(BACKGROUND_COLOR);
	display_menu_conf(info);
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
		      Fade(BLACK, 0.7f));
	set_start_coords_grid(info.width, info.height);
	grid_draw(info.width, info.height, p.start_x, p.start_y,
		  p.board_wall_thickness, p.delta, GRID_COLOR);
	char title_height[] = "Set board height:";
	DrawText(title_height,
		 p.screen_width / 2 -
		     MeasureText(title_height, p.font_size_big) / 2,
		 p.screen_height / 4, p.font_size_big, TEXT_COLOR);
	char height_number_string[5];
	char height_string[] = " tiles high";
	sprintf(height_number_string, "%d", info.height);
	int const height_number_string_len =
	    MeasureText(height_number_string, p.font_size_big);
	int const height_string_len =
	    MeasureText(height_string, p.font_size_big);
	DrawText(height_number_string,
		 p.screen_width / 2 -
		     (height_number_string_len + height_string_len) / 2,
		 40 + p.screen_height / 4, p.font_size_big, BLUE);
	DrawText(height_string,
		 p.screen_width / 2 -
		     (height_number_string_len + height_string_len) / 2 +
		     height_number_string_len,
		 40 + p.screen_height / 4, p.font_size_big, MAROON);
}

void display_wrapping_conf(DisplayConfigureInfo const info) {
	display_menu_conf(info);
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
		      Fade(BLACK, 0.7f));
	board_draw(info.demo, 0, false, false);
}
void display_snake_speed_conf(DisplayConfigureInfo const info) {

	ClearBackground(BACKGROUND_COLOR);
	display_menu_conf(info);
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
		      Fade(BLACK, 0.7f));
	board_draw(info.demo, 0, false, false);
	char title_speed[] = "Set snake speed:";
	DrawText(title_speed,
		 p.screen_width / 2 -
		     MeasureText(title_speed, p.font_size_big) / 2,
		 p.screen_height / 4, p.font_size_big, TEXT_COLOR);
	char speed_number_string[5];
	char speed_string[] = " ticks/second (Hz)";
	sprintf(speed_number_string, "%.2f", info.freq);
	int const speed_number_string_len =
	    MeasureText(speed_number_string, p.font_size_big);
	int const speed_string_len = MeasureText(speed_string, p.font_size_big);
	DrawText(speed_number_string,
		 p.screen_width / 2 -
		     (speed_number_string_len + speed_string_len) / 2,
		 40 + p.screen_height / 4, p.font_size_big, BLUE);
	DrawText(speed_string,
		 p.screen_width / 2 -
		     (speed_number_string_len + speed_string_len) / 2 +
		     speed_number_string_len,
		 40 + p.screen_height / 4, p.font_size_big, MAROON);
}

void display_high_score(HighScoreEntry const *h, int const num_entries) {
	ClearBackground(BACKGROUND_COLOR);
	char title[] = "Highscores";
	int const center_x_title =
	    (p.screen_width - MeasureText(title, p.font_size_big)) / 2;
	DrawText(title, center_x_title, 0.1 * p.screen_height, p.font_size_big,
		 TEXT_COLOR);
	if (h) {
		int const interval = 0.1 * p.screen_height;
		for (int i = 0; i < num_entries; i++) {
			char score_str[10] = "";
			sprintf(score_str, "%d", h[i].score);
			int const margin = p.screen_width / 8;
			int const y_text =
			    0.1 * p.screen_height + p.font_size_big + 30 * i;
			if (y_text > p.screen_height)
				break;
			DrawText(h[i].name,
				 margin -
				     MeasureText(h[i].name, p.font_size_small),
				 y_text, p.font_size_small, TEXT_COLOR);
			DrawText(score_str,
				 p.screen_width - margin -
				     MeasureText(score_str, p.font_size_small),
				 y_text, p.font_size_small, TEXT_COLOR);
		}
	}
}

void get_screen_measurements() {
	p.screen_width = GetScreenWidth();
	p.screen_height = GetScreenHeight();
}

void board_draw_collision(Board const *const b, int const board_x,
			  int const board_y) {
	draw_square(&p, board_x, board_y, RED);
}

void window_periodic_end() {
	if (p.draw_fps)
		DrawFPS(10, 10);
	EndDrawing();
}

bool is_display_name_box_overflown(char *name) {
	// 225, 50
	int text_width = MeasureText(name, p.font_size_big);
	if (text_width < 225) {
		return false;
	}
	return true;
}
