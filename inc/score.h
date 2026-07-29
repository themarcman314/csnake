#pragma once
#include <stdio.h>
#if defined(PLATFORM_WEB)
#include <emscripten/fetch.h>
#endif

typedef struct {
	char name[50];
	int score;
} HighScoreEntry;

int count_lines_string(char const *string, int size);
void parse_high_score_entries(char const *s, HighScoreEntry *h,
			      int const entry_count);
void save_score(char const *name, unsigned const score);
void sort_highscore_entries(HighScoreEntry *h, int const num_entries);

void downloadSucceeded(emscripten_fetch_t *fetch);
void downloadFailed(emscripten_fetch_t *fetch);
