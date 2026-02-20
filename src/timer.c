#ifdef _WIN32
#error "This program does not support windows :("
#else
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
