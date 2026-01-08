#ifndef TIME_MAIN_HEADER
#include <mds/genmod.h>
#include "timing.h"

typedef struct {
    Module _minfo;

    i64 (*fromSeconds)(i64  s);
    i64 (*fromMicros) (i64 us);
    i64 (*fromMillis) (i64 ms);
    i64 (*asSeconds)  (i64 ns);
    i64 (*asMicros)   (i64 ns);
    i64 (*asMillis)   (i64 ns);
    option (*current) (TIME_TYPE type);
    void (*sleep)(i64 ns);
    
    struct timespec (*asPosix)(i64 ns_time);
} std_time;
std_time __std_time_struct;

void __time_setup(){
    __std_time_struct._minfo = mModuleNew("std.time");
    __std_time_struct.fromSeconds = __time_from_seconds;
    __std_time_struct.fromMicros  = __time_from_microseconds;
    __std_time_struct.fromMillis  = __time_from_milliseconds;
    __std_time_struct.asSeconds   = __time_as_seconds;
    __std_time_struct.asMicros = __time_as_milliseconds;
    __std_time_struct.asMillis = __time_as_milliseconds;
    __std_time_struct.asPosix  = __time_as_posix;
    __std_time_struct.current  = __time_currentnanosecs;
    __std_time_struct.sleep    = __time_sleep;
}

#define TIME_INJECT_MOD {__std_time_struct._minfo, &__std_time_struct}
#endif
#define TIME_MAIN_HEADER
