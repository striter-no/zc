#pragma once
#include <mds/genmod.h>
#include "signals.h"

typedef struct {
    Module _minfo;

    void   (*ignore)(NAMED_SIGNALS vsig);
    void   (*set_default)(NAMED_SIGNALS vsig);
    option (*new_handler)(NAMED_SIGNALS vsig, void (*handler)(int sign));
    void   (*raise)(NAMED_SIGNALS vsig);
    void   (*kill)(NAMED_SIGNALS vsig, pid_t pid);
    void   (*thr_raise)(NAMED_SIGNALS vsig, pthread_t thr);
} std_signal;
std_signal __std_signal_struct;

void __signal_setup(){
    __std_signal_struct._minfo = mModuleNew("std.signal");
    __std_signal_struct.ignore = __signal_ignore;
    __std_signal_struct.set_default = __signal_default;
    __std_signal_struct.new_handler = __signal_register;
    __std_signal_struct.raise = __signal_raise;
    __std_signal_struct.kill = __signal_kill;
    __std_signal_struct.thr_raise = __signal_thr_raise;
}

#define SIGNAL_INJECT_MOD {__std_signal_struct._minfo, &__std_signal_struct}