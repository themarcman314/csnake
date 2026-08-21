#include "score.h"
#include "conf.h"
#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t parse_name_high_score_entry(char const *line_start, char *name);
size_t parse_score_high_score_entry(char const *score_start, int *score);

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

#ifdef PLATFORM_WEB
void score_sent_success(emscripten_fetch_t *fetch) {
	printf("Score was sent!\n");
}
void score_sent_fail(emscripten_fetch_t *fetch) {
	printf("Score was not sent\n");
}
#endif

// FIXME: implement version with sockets for web version
//  highscores should be saved on vps not on client machine lol
void save_score(char const *name, unsigned const score) {
#ifdef PLATFORM_WEB
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);

	// set request method
	strcpy(attr.requestMethod, "POST");

	// add headers
	// const char *headers[] = {"Content-Type", "application/json", NULL};
	attr.requestHeaders = NULL;
	char *json_payload = malloc(256);
	snprintf(json_payload, 256, "{\"name\": \"%s\", \"score\": %u}", name,
		 score);
	// sprintf(json_payload, "{\"name\": \"%s\", \"score\": %u}", name,
	// score);
	printf("payload:\n%s\n", json_payload);
	attr.requestData = json_payload;
	attr.requestDataSize = strlen(json_payload);
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = score_sent_success;
	attr.onerror = score_sent_fail;

	emscripten_fetch(&attr, "/submit_score");
#endif
}

void parse_high_score_entries(char const *string, HighScoreEntry *h,
			      int const entry_count) {
	for (int i = 0; i < entry_count; i++)
		memset(h[i].name, 0, sizeof(h->name));
	char const *cursor = string;
	for (int i = 0; i < entry_count; i++) {
		size_t consumed =
		    parse_name_high_score_entry(cursor, h[i].name);
		if (consumed > 0) {
			cursor += consumed;
			printf("name is: %s\n", h[i].name);
		}
		if (*cursor == ',')
			cursor++;
		consumed = parse_score_high_score_entry(cursor, &h[i].score);
		if (consumed > 0) {
			cursor += consumed;
			printf("num_digits: %lu\n", consumed);
			printf("score: %d\n", h[i].score);
		}
		while (*cursor != '\n')
			cursor++;
		cursor++; // ignore '\n' and go to next line
	}
}

size_t parse_name_high_score_entry(char const *line_start, char *name) {
	char const *cursor = line_start;
	int namesize = 0;
	while (*cursor != ',') {
		cursor++;
		namesize++;
	}
	memcpy(name, line_start, namesize);
	name[namesize] = '\0';
	return namesize;
}

size_t parse_score_high_score_entry(char const *score_start, int *score) {
	*score = atoi((char *)(score_start));
	char const *cursor = score_start;
	size_t num_digits = 0;
	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		num_digits++;
		cursor++;
	}
	return num_digits;
}

int count_lines_string(char const *string, int size) {
	int num_lines = 0;
	for (int i = 0; i < size; i++) {
		if (string[i] == '\n')
			num_lines++;
	}
	return num_lines;
}

#ifdef PLATFORM_WEB
void downloadSucceeded(emscripten_fetch_t *fetch) {
	printf("Finished downloading %llu bytes from URL %s.\n",
	       fetch->numBytes, fetch->url);
	Game *g = fetch->userData;
	HighScoreEntry *entries = g->high_scores;

	g->num_high_scores = 0;
	g->num_high_scores = count_lines_string(fetch->data, fetch->numBytes);

	if (entries != NULL) {
		free(entries);
		g->high_scores = NULL;
	}

	HighScoreEntry *new_entries =
	    malloc(sizeof(HighScoreEntry) * g->num_high_scores);
	if (new_entries) {
		printf("allocated mem for entries\n");
		parse_high_score_entries(fetch->data, new_entries,
					 g->num_high_scores);
		sort_highscore_entries(new_entries, g->num_high_scores);
		g->high_scores = new_entries;
	}
	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}
void downloadFailed(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\n",
	       fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
}
#endif
