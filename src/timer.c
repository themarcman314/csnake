#ifdef _WIN32
#error "This program does not support windows :("
#else
#define _POSIX_C_SOURCE 199309L // Enables POSIX features like clock_gettime
#include <stdint.h>
#include <time.h>
#endif

int millis(void) {
	int ms;
#ifdef _WIN32
#error "This program does not support windows :("
#else
	// printf("this is posix system");
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ms = ts.tv_sec *
		 1000LL + // ensure 64 bit arithmetic to avoid multiply overflow
	     ts.tv_nsec / 1000000;
#endif
	return ms;
}
