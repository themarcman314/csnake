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

void parse_high_score_entries(char const *string, HighScoreEntry *h,
			      int const entry_count) {
	for (int i = 0; i < entry_count; i++)
		memset(h[i].name, 0, sizeof(h->name));
	int namesize = 0;
	char *start_line = string;
	for (int i = 0; i < entry_count; i++) {
		// fgets(line, line_size, f);
		char *c = start_line;
		while (*c != ',') {
			c++;
			namesize++;
		}
		memcpy(h[i].name, start_line, namesize);
		h[i].name[namesize] = '\0';
		namesize = 0;
		h[i].score = atoi((char *)(c + 1));
		while (*start_line != '\n')
			start_line++;
		start_line++; // ignore '\n' and go to next line
	}
}

int count_lines_string(char const *string, int size) {
	int num_lines = 0;
	for (int i = 0; i < size; i++) {
		if (string[i] == '\n')
			num_lines++;
	}
	return num_lines;
}
