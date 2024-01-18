#include "IZ_timer.h"

#if defined IZ_WINDOWS
typedef enum {
	_CLOCK_REALTIME = 0,
#define CLOCK_REALTIME _CLOCK_REALTIME
	_CLOCK_MONOTONIC = 6,
#define CLOCK_MONOTONIC _CLOCK_MONOTONIC
} clockid_t;

int clock_gettime(clockid_t __clock_id, struct timespec *__tp) {
	return timespec_get(__tp, TIME_UTC);
}
#endif

/**
 * Gets the start timestamp.
 * @sa IZ_TIMER_START_TIMESTAMP
 */
void IZ_TimerStart() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	IZ_TIMER_START_TIMESTAMP = t.tv_sec;
}

/**
 * Gets the number of microseconds since the application timer has been started.
 * @return The number of microseconds.
 * @sa IZ_TimerElapsed()
 */
unsigned int IZ_TimerElapsedRaw() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_sec - IZ_TIMER_START_TIMESTAMP) * 1000000 + (t.tv_nsec / 1000);
}

/**
 * Gets the formatted elapsed time since the application timer has been started.
 * @return The formatted elapsed time.
 * @sa IZ_TimerElapsedRaw()
 */
char* IZ_TimerElapsed() {
	static char buffer[48];
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	unsigned int seconds = t.tv_sec - IZ_TIMER_START_TIMESTAMP;
	unsigned int milliseconds = t.tv_nsec / 1000000;
	unsigned int minutes = seconds / 60;
	unsigned int hours   = seconds / 60 / 60;
	sprintf(buffer, "%02d:%02d:%02d.%03d", hours, minutes % 60, seconds % 60, milliseconds % 1000);
	return buffer;
}

/**
 * Gets the formatted time in the current instant.
 * @return The formatted time in the current instant.
 */
char* IZ_TimerNow() {
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	static char buffer[48];
	unsigned int seconds = t.tv_sec;
	unsigned int milliseconds = t.tv_nsec / 1000000;
	static char formatted[32];
	time_t current_time = seconds;
	strftime(formatted, sizeof(formatted), "%Y-%m-%dT%H:%M:%S", gmtime(&current_time));
	sprintf(buffer, "%s.%03dZ", formatted, milliseconds < 0 ? 0 : milliseconds % 1000);
	return buffer;
}

/**
 * Gets the formatted time in the current instant that can be safely added in file paths.
 * @return The formatted time in the current instant.
 */
char* IZ_TimerNowPathSafe() {
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	static char buffer[48];
	unsigned int seconds = t.tv_sec;
	unsigned int milliseconds = t.tv_nsec / 1000000;
	static char formatted[32];
	time_t current_time = seconds;
	strftime(formatted, sizeof(formatted), "%Y%m%d%H%M%S", gmtime(&current_time));
	sprintf(buffer, "%s%03d", formatted, milliseconds < 0 ? 0 : milliseconds % 1000);
	return buffer;
}
