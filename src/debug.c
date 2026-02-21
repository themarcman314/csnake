#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FILENAME "./snake.log"

void log_to_file(char const *file, int line, char const *format, ...) {
#ifdef DEBUG
	va_list list;
	va_start(list, format);
	FILE *f;
	if ((f = fopen(FILENAME, "a")) == NULL) {
		fprintf(stderr, "%s", "could not open log file");
		exit(EXIT_FAILURE);
	}
	fprintf(f, "[%s:%d] ", file, line);
	vfprintf(f, format, list);
	fputc('\n', f);
	va_end(list);
	fclose(f);
#endif
}

void debug_init() {
	remove(FILENAME);
}
