#ifndef IZ_TIMER_H
#define IZ_TIMER_H

#include <stdio.h>
#include <time.h>

/**
 * The start timestamp.
 * @sa IZ_TimerStart
 */
static time_t IZ_TIMER_START_TIMESTAMP;

/**
 * Gets the start timestamp.
 * @sa IZ_TIMER_START_TIMESTAMP
 */
void IZ_TimerStart(void);

/**
 * Gets the number of microseconds since the application timer has been started.
 * @return The number of microseconds.
 * @sa IZ_TimerElapsed()
 */
unsigned int IZ_TimerElapsedRaw(void);

/**
 * Gets the formatted elapsed time since the application timer has been started.
 * @return The formatted elapsed time.
 * @sa IZ_TimerElapsedRaw()
 */
char* IZ_TimerElapsed(void);

/**
 * Gets the formatted time in the current instant.
 * @return The formatted time in the current instant.
 */
char* IZ_TimerNow(void);

/**
 * Gets the formatted time in the current instant that can be safely added in file paths.
 * @return The formatted time in the current instant.
 */
char* IZ_TimerNowPathSafe(void);

#endif
