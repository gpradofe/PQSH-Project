/* timestamp.c: PQSH Timestamp */

#include "pqsh/timestamp.h"

#include <time.h>
#include <sys/time.h>

/**
 * Return current timestamp as a double.
 *
 * Utilizes gettimeofday for precision (accounting for seconds and
 * microseconds) and falls back to time (only account for seconds) if that
 * fails.
 *
 * @return  Double representing the current time.
 **/
double timestamp() {
    struct timeval tv;

    // Try to get the time with microsecond precision
    if (gettimeofday(&tv, NULL) == 0) {
        return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    }

    // If gettimeofday failed, fall back to second precision
    time_t t = time(NULL);
    if (t != (time_t)-1) {
        return (double)t;
    }

    // If even the fallback failed (very unlikely), return 0
    return 0;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
