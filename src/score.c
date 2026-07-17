#include "score.h"
#include "conf.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void sort_highscore_entries(HighScoreEntry *h, int const num_entries) {
	HighScoreEntry temp;
	bool sorted = true;
	do {
		sorted = true;
		for (int i = 0; i < num_entries - 1; i++) {
			HighScoreEntry *current = h + i;
			HighScoreEntry *next = current + 1;
			if (current->score < next->score) {
				sorted = false;
				memcpy(&temp, current, sizeof(HighScoreEntry));
				memcpy(current, next, sizeof(HighScoreEntry));
				memcpy(next, &temp, sizeof(HighScoreEntry));
			}
		}
	} while (!sorted);
}

// FIXME: implement version with sockets for web version
//  highscores should be saved on vps not on client machine lol
void save_score(char const *name, unsigned const score) {
	FILE *f = fopen(HIGH_SCORE_FILE_PATH, "a");
	if (f) {
		// fseek(f, 0, SEEK_END);
		if (strlen(name) > 0)
			fprintf(f, "%s,%d\n", name, score);
		fclose(f);
	}
}

void parse_high_score_entries(FILE *f, HighScoreEntry *h,
			      int const entry_count) {
	for (int i = 0; i < entry_count; i++)
		memset(h[i].name, 0, sizeof(h->name));
	int const line_size = 100;
	char line[line_size];
	for (int i = 0; i < entry_count; i++) {
		fgets(line, line_size, f);
		char *end_line = line;
		while (*end_line != ',')
			end_line++;
		char name[20] = "";
		memcpy(h[i].name, line, end_line - line);
		h[i].score = atoi((char *)(end_line + 1));
	}
}

int count_lines_file(FILE *f) {
	fseek(f, 0, SEEK_SET); // move to start of file
	char c;
	int num_lines = 0;
	while ((c = fgetc(f)) != EOF) {
		if (c == '\n')
			num_lines++;
	}
	fseek(f, 0, SEEK_SET); // move to start of file
	return num_lines;
}
