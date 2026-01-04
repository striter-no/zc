#pragma once
#include <mds/genmod.h>
#include "term.h"
#include "stream.h"
#include "polling.h"
#include "colors.h"

typedef struct {
    Module _minfo;

    stream (*openStream)(int fd, size_t read_bfsize, size_t write_bfsize);
    void (*closeStream)(stream *str);

    option (*sread)(stream str);
    option (*swrite)(stream str, u8* data, size_t actual_sz);
    option (*aio_sread)(stream str);
    option (*aio_swrite)(stream str, u8* data, size_t actual_sz);
} stream_io;
stream_io __stream_io;

typedef struct {
    Module _minfo;

    option (*init)();
    option (*close)(epoller eplr);
    option (*modify)(epoller eplr, int fd_to_mod, u32 new_events, void *dataptr);
    option (*add)(epoller eplr, int fd_to_add, u32 events, void *dataptr);
    option (*delete)(epoller eplr, int fd_to_del);
    option (*waitev)(epoller *eplr, int timeout);
} epoll_io;
epoll_io __epoll_io;

typedef struct {
    Module _minfo;

    void (*print)(const char*, ...);
    void (*println)(const char*, ...);
    option (*input)(const char *queue);
} term_io;
term_io __term_io;


void __si_setup(){
    __ansi_setup();

    __term_io._minfo = mModuleNew("std.io.term");
    __term_io.print = __print;
    __term_io.println = __println;
    __term_io.input = __input;

    __stream_io._minfo = mModuleNew("std.io.sio");
    __stream_io.openStream = __openStream;
    __stream_io.swrite = __swrite;
    __stream_io.sread = __sread;
    __stream_io.aio_swrite = __aio_swrite;
    __stream_io.aio_sread = __aio_sread;
    __stream_io.closeStream = __closeStream;

    __epoll_io._minfo = mModuleNew("std.io.epoll");
    __epoll_io.init = __epoller_init;
    __epoll_io.close = __epoller_close;
    __epoll_io.modify = __epoller_modify;
    __epoll_io.add = __epoller_add;
    __epoll_io.delete = __epoller_delete;
    __epoll_io.waitev = __epoller_waitev;
}

#define TERMIO_INJECT_MOD {__term_io._minfo, &__term_io}
#define STREAMIO_INJECT_MOD {__epoll_io._minfo, &__epoll_io}
#define EPOLLIO_INJECT_MOD {__stream_io._minfo, &__stream_io}
