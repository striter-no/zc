#ifndef SIGNALS_HEADER
#include "mds/std/_preincl/base/preproc.h"
#include <signal.h>
#include <unistd.h>
#include <mds/std/_preincl/base.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>

typedef enum {
    SIG_HUP  = 1,   // Hangup (разрыв терминала)
    SIG_INT  = 2,   // Interrupt (Ctrl+C)
    SIG_QUIT = 3,   // Quit (Ctrl+\)
    SIG_ILL  = 4,   // Illegal instruction
    SIG_ABRT = 6,   // Abort
    SIG_FPE  = 8,   // Floating point exception
    SIG_KILL = 9,   // Kill (нельзя перехватить!)
    SIG_SEGV = 11,   // Segmentation fault
    SIG_PIPE = 13,   // Broken pipe
    SIG_ALRM = 14,   // Alarm clock
    SIG_TERM = 15,   // Termination (graceful shutdown)
    SIG_USR1 = 10,   // User-defined 1
    SIG_USR2 = 12,   // User-defined 2
    SIG_CHLD = 17,   // Child status changed
    SIG_CONT = 18,   // Continue if stopped
    SIG_STOP = 19,   // Stop (нельзя перехватить!)
    SIG_TSTP = 2   // Terminal stop (Ctrl+Z)
} NAMED_SIGNALS;

void __signal_ignore(NAMED_SIGNALS vsignal);
void __signal_default(NAMED_SIGNALS vsignal);
option __signal_register(NAMED_SIGNALS vsignal, void (*handler)(int sign));
void __signal_raise(NAMED_SIGNALS vsignal);
void __signal_kill(NAMED_SIGNALS vsignal, pid_t pid);
void __signal_thr_raise(NAMED_SIGNALS vsignal, pthread_t thr);
#ifdef SIGNALS_IMPLEMENTATION

void __signal_default(NAMED_SIGNALS vsignal){
    signal(vsignal, SIG_DFL);
}

void __signal_kill(NAMED_SIGNALS vsignal, pid_t pid){
    kill(pid, vsignal);
}

void __signal_raise(NAMED_SIGNALS vsignal){
    raise(vsignal);
}

void __signal_thr_raise(NAMED_SIGNALS vsignal, pthread_t thr){
    pthread_kill(thr, vsignal);
}

void __signal_ignore(NAMED_SIGNALS vsignal){
    signal(vsignal, SIG_IGN);
}

option __signal_register(NAMED_SIGNALS vsignal, void (*handler)(int sign)){
    if (!handler) throw(
        "Std.Signal failed to register new handler, handler is NULL", 
        "Std.Signal.Handler.IsNULL", 
        -1
    );

    signal(vsignal, handler);
    return noerropt;
}


#endif
#endif
#define SIGNALS_HEADER
