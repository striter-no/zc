#ifndef TIMING_HEADER
#include "mds/std/_preincl/base/preproc.h"
#include <errno.h>
#include <time.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>

// all timing is in nanoseconds

typedef enum {
    ETIME_MONOTONIC,ETIME_SYSTEM,ETIME_MONOTONIC_FAST,ETIME_PROC,ETIME_THREAD
} TIME_TYPE;

struct timespec __time_as_posix(i64 ns_time);
option __time_currentnanosecs(TIME_TYPE type);
i64 __time_as_milliseconds(i64 ns);
i64 __time_as_microseconds(i64 ns);
i64 __time_as_seconds     (i64 ns);

i64 __time_from_milliseconds(i64 ms);
i64 __time_from_microseconds(i64 us);
i64 __time_from_seconds     (i64  s);

void __time_sleep(i64 ns)     ;
#ifdef TIMING_IMPLEMENTATION

void __time_sleep(i64 ns)     {
    var req = __time_as_posix(ns);
    struct timespec rem = {0};

    while (nanosleep(&req, &rem) == -1 && errno == EINTR){
        req = rem;
    }
}

i64 __time_as_seconds     (i64 ns){
	return ns * 0.000000001;
}

i64 __time_as_milliseconds(i64 ns){
	return ns * 0.000001;
}

struct timespec __time_as_posix(i64 ns_time){
    struct timespec ts;
    ts.tv_sec = ns_time / 1000000000L;
    ts.tv_nsec = ns_time % 1000000000L;

    return ts;
}

i64 __time_from_milliseconds(i64 ms){
	return ms * 1000000;
}

option __time_currentnanosecs(TIME_TYPE type){
    struct timespec ts;
    clockid_t clock_id;
    
    switch (type){
        case ETIME_MONOTONIC:      clock_id = CLOCK_MONOTONIC; break;
        case ETIME_MONOTONIC_FAST: 
            #ifdef CLOCK_MONOTONIC_COARSE
                clock_id = CLOCK_MONOTONIC_COARSE; 
            #else
                clock_id = CLOCK_MONOTONIC;
            #endif
            break;
        case ETIME_SYSTEM:         clock_id = CLOCK_REALTIME; break;
        case ETIME_PROC:           clock_id = CLOCK_PROCESS_CPUTIME_ID; break;
        case ETIME_THREAD:         clock_id = CLOCK_THREAD_CPUTIME_ID; break;
        default: unreachable;
    }
    
    if (clock_gettime(clock_id, &ts) != 0){
        throw(        "Time: current nanonesconds failed, clock_gettime() failed",        "Std.Time.ClockGettime.Failed",        -1
        )
    }
    
    return opt(NULL, ts.tv_sec * 1000000000L + ts.tv_nsec, false);
}

i64 __time_from_microseconds(i64 us){
	return us * 1000;
}

i64 __time_as_microseconds(i64 ns){
	return ns * 0.001;
}

i64 __time_from_seconds     (i64  s){
	return s  * 1000000000;
}


#endif
#endif
#define TIMING_HEADER
