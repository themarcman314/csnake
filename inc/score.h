#pragma once
#include <stdio.h>

typedef struct {
	char name[50];
	int score;
} HighScoreEntry;

int count_lines_file(FILE *f);
void parse_high_score_entries(FILE *f, HighScoreEntry *h,
			      int const entry_count);
void save_score(char const *name, unsigned const score);
void sort_highscore_entries(HighScoreEntry *h, int const num_entries);
