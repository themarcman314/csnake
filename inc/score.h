#pragma once
#include <stdbool.h>
#if defined(PLATFORM_WEB)
#include <emscripten/fetch.h>
#endif

typedef struct {
	int rank;
	char name[50];
	int score;
	bool board_wrapping;
	int board_width;
	int board_height;
} HighScoreEntry;

int count_lines_string(char const *string, int size);
void parse_high_score_entries(char const *s, HighScoreEntry *h,
			      int const entry_count);
void sort_highscore_entries(HighScoreEntry *h, int const num_entries);

#if defined(PLATFORM_WEB)
void downloadSucceeded(emscripten_fetch_t *fetch);
void downloadFailed(emscripten_fetch_t *fetch);
#endif
