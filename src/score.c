#include "score.h"
#include "conf.h"
#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

size_t parse_name_high_score_entry(char const *line_start, char *name);
size_t parse_score_high_score_entry(char const *score_start, int *score);
size_t parse_board_wrapping_high_score_entry(char const *score_start,
					     bool *board_wrapping);
size_t parse_board_dimentions_high_score_entry(char const *score_start,
					       int *width, int *height);
size_t parse_time_high_score_entry(char const *start, long long *timestamp);
void assign_ranks(HighScoreEntry *h, int num_entries);

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

void assign_ranks(HighScoreEntry *h, int num_entries) {
	for (int i = 0; i < num_entries; i++)
		h[i].rank = i + 1;
}

#ifdef PLATFORM_WEB
void score_sent_success(emscripten_fetch_t *fetch) {
	printf("Score was sent!\n");
}
void score_sent_fail(emscripten_fetch_t *fetch) {
	printf("Score was not sent\n");
}

void country_code_dl_success(emscripten_fetch_t *fetch) {
	char *code = fetch->userData;
	char const *cursor = fetch->data;
	char const success_str[] = "success";
	if (memcmp(cursor, success_str, sizeof success_str - 1) == 0) {
		cursor += sizeof success_str;
		if (*cursor == ',')
			cursor++;
		memcpy(code, cursor, 2);
	}
}
void country_code_dl_failed(emscripten_fetch_t *fetch) {
	printf("Unable to download country code\n");
}

#endif

void save_score(Game const *g) {
#ifdef PLATFORM_WEB
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);

	// set request method
	strcpy(attr.requestMethod, "POST");

	attr.requestHeaders = NULL;
	char *json_payload = calloc(256, 1);
	if (json_payload != NULL) {
		time_t t = time(NULL);
		snprintf(
		    json_payload, 256,
		    "{\"name\": \"%s\", \"score\": %u, \"board wrapping\": %d, "
		    "\"board width\": %d, \"board height\": %d, \"timestamp\": "
		    "%lld, \"country code\": \"%s\"}",
		    g->player_name, g->score, g->wrapping, g->b->width,
		    g->b->height, t, g->country_code);
		printf("payload:\n%s\n", json_payload);
		attr.requestData = json_payload;
		attr.requestDataSize = strlen(json_payload);
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		attr.onsuccess = score_sent_success;
		attr.onerror = score_sent_fail;

		emscripten_fetch(&attr, "/submit_score");
	} else {
		fprintf(stderr, "failed to allocate memory for post request\n");
	}
#endif
}

void fill_country_code(char *code) {
#ifdef PLATFORM_WEB
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	attr.userData = code;
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = country_code_dl_success;
	attr.onerror = country_code_dl_failed;

	emscripten_fetch(&attr,
			 "http://ip-api.com/csv/?fields=status,countryCode");
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
		if (*cursor == ',') {
			cursor++;
		}
		consumed = parse_board_wrapping_high_score_entry(
		    cursor, &h[i].board_wrapping);
		if (consumed > 0) {
			cursor += consumed;
			printf("board_wrapping: %s\n",
			       h[i].board_wrapping ? "true" : "false");
		}
		if (*cursor == ',') {
			cursor++;
		}
		consumed = parse_board_dimentions_high_score_entry(
		    cursor, &h[i].board_width, &h[i].board_height);
		if (consumed > 0) {
			cursor += consumed;
			printf("width: %d, height: %d\n", h[i].board_width,
			       h[i].board_height);
		}
		if (*cursor == ',') {
			cursor++;
		}
		consumed = parse_time_high_score_entry(cursor, &h[i].timestamp);
		if (consumed > 0) {
			cursor += consumed;
		}

		if (*cursor == ',') {
			cursor++;
		}
		while (*cursor == ' ')
			cursor++;
		memcpy(h[i].country_code, cursor, 2);
		printf("found country... %s\n", h->country_code);

		while (*cursor != '\n')
			cursor++;
		cursor++; // ignore '\n' and go to next line
	}
}

size_t parse_name_high_score_entry(char const *line_start, char *name) {
	char const *cursor = line_start;
	while (*cursor != ',') {
		cursor++;
	}
	size_t name_size = cursor - line_start;
	memcpy(name, line_start, name_size);
	name[name_size] = '\0';
	return name_size;
}

size_t parse_time_high_score_entry(char const *start, long long *timestamp) {
	char const *cursor = start;
	while (*cursor == ' ')
		cursor++;
	*timestamp = atoll(cursor);
	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		cursor++;
	}
	return cursor - start;
}

size_t parse_score_high_score_entry(char const *score_start, int *score) {
	*score = atoi((char *)(score_start));
	char const *cursor = score_start;
	while (*cursor == ' ')
		cursor++;
	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		cursor++;
	}
	return cursor - score_start;
}
size_t parse_board_wrapping_high_score_entry(char const *start,
					     bool *board_wrapping) {
	char const *cursor = start;
	while (*cursor == ' ')
		cursor++;
	if (atoi(cursor) == true)
		*board_wrapping = true;
	else
		*board_wrapping = false;

	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		cursor++;
	}
	return cursor - start;
}

size_t parse_board_dimentions_high_score_entry(char const *start, int *width,
					       int *height) {

	char const *cursor = start;
	while (*cursor == ' ')
		cursor++;
	*width = atoi(cursor);
	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		cursor++;
	}
	while (*cursor == ' ')
		cursor++;
	if (*cursor == ',')
		cursor++;
	while (*cursor == ' ')
		cursor++;
	*height = atoi(cursor);
	while (*cursor >= 0x30 && *cursor <= 0x39) { // is digit
		cursor++;
	}
	while (*cursor == ' ')
		cursor++;
	return cursor - start;
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
	printf("Finished downloading %lu bytes from URL %s.\n", fetch->numBytes,
	       fetch->url);
	Game *g = fetch->userData;
	HighScoreEntry *entries = g->high_scores;

	g->num_high_scores = 0;
	g->num_high_scores = count_lines_string(fetch->data, fetch->numBytes);

	if (entries != NULL) {
		free(entries);
		g->high_scores = NULL;
	}

	HighScoreEntry *new_entries =
	    calloc(sizeof(HighScoreEntry) * g->num_high_scores, 1);
	if (new_entries) {
		printf("allocated mem for entries\n");
		for (int i = 0; i < fetch->numBytes; i++)
			printf("%c", fetch->data[i]);
		parse_high_score_entries(fetch->data, new_entries,
					 g->num_high_scores);
		sort_highscore_entries(new_entries, g->num_high_scores);
		assign_ranks(new_entries, g->num_high_scores);
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
