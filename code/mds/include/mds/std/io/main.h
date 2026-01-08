#ifndef IO_MAIN_HEADER
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
    option (*nbio_sread)(stream str);
    option (*nbio_swrite)(stream str, u8* data, size_t actual_sz);
} std_io_stream;
std_io_stream __std_io_stream;

typedef struct {
    Module _minfo;

    option (*init)(void *issuer);
    option (*close)(epoller eplr);
    option (*modify)(epoller eplr, int fd_to_mod, u32 new_events, void *dataptr);
    option (*add)(epoller eplr, int fd_to_add, u32 events, void *dataptr);
    option (*delete)(epoller eplr, int fd_to_del);
    option (*waitev)(epoller *eplr, int timeout);
} std_io_epoll;
std_io_epoll __std_io_epoll;

typedef struct {
    Module _minfo;

    void (*print)(const char*, ...);
    void (*println)(const char*, ...);
    option (*input)(const char *queue);
} std_io_term;
std_io_term __std_io_term;


void __si_setup(){
    __ansi_setup();

    __std_io_term._minfo = mModuleNew("std.io.term");
    __std_io_term.print = __print;
    __std_io_term.println = __println;
    __std_io_term.input = __input;

    __std_io_stream._minfo = mModuleNew("std.io.sio");
    __std_io_stream.openStream = __openStream;
    __std_io_stream.swrite = __swrite;
    __std_io_stream.sread = __sread;
    __std_io_stream.nbio_swrite = __nbio_swrite;
    __std_io_stream.nbio_sread = __nbio_sread;
    __std_io_stream.closeStream = __closeStream;

    __std_io_epoll._minfo = mModuleNew("std.io.epoll");
    __std_io_epoll.init = __epoller_init;
    __std_io_epoll.close = __epoller_close;
    __std_io_epoll.modify = __epoller_modify;
    __std_io_epoll.add = __epoller_add;
    __std_io_epoll.delete = __epoller_delete;
    __std_io_epoll.waitev = __epoller_waitev;
}

#define TERMIO_INJECT_MOD {__std_io_term._minfo, &__std_io_term}
#define STREAMIO_INJECT_MOD {__std_io_epoll._minfo, &__std_io_epoll}
#define EPOLLIO_INJECT_MOD {__std_io_stream._minfo, &__std_io_stream}

#endif
#define IO_MAIN_HEADER
