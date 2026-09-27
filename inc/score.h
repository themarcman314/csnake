#pragma once
#include <stdbool.h>
#include <stddef.h>
#if defined(PLATFORM_WEB)
#include <emscripten/fetch.h>
#endif
#include "direction.h"

typedef struct {
	int rank;
	char name[50];
	int score;
	bool board_wrapping;
	int board_width;
	int board_height;
	long long timestamp;
	char country_code[2];
} HighScoreEntry;

typedef struct {
	int tick;
	Direction dir;
} InputEvent;

typedef struct InputLog {
	InputEvent *events;
	size_t capacity; // total allocated number of InputEvents
	size_t count;	 // currently used
} InputLog;

int count_lines_string(char const *string, int size);
void parse_high_score_entries(char const *s, HighScoreEntry *h,
			      int const entry_count);
void sort_highscore_entries(HighScoreEntry *h, int const num_entries);
void fill_country_code(char *code);

#if defined(PLATFORM_WEB)
void downloadSucceeded(emscripten_fetch_t *fetch);
void downloadFailed(emscripten_fetch_t *fetch);

InputLog *input_log_create(void);
void input_log_push(InputLog *log, int tick, Direction dir);
void input_log_delete(InputLog *buff);

#endif
