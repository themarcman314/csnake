#ifdef _WIN32
#error "This program does not support windows :("
#else
#define _POSIX_C_SOURCE 199309L // Enables POSIX features like clock_gettime
#include <stdint.h>
#include <time.h>

uint32_t millis(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t sec_ms =
	    (uint64_t)ts.tv_sec * 1000ULL; // ensure promotion to 64 bit to
					   // avoid 2038 unix time problem
	uint32_t ns_ms =
	    (uint32_t)ts.tv_nsec / 1000000U; // 10^9 fits well within 32 bits
	return (uint32_t)(sec_ms + ns_ms); // accept overflow at 49 day machine
					   // uptime (player's machine will
					   // likely not be that long)
}
#endif
