#pragma once
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/memory/allocators/abstract.h>

#define MAX_BUFFER_SIZE 1024
typedef struct {
    int    fd;
    size_t read_bfsize;
    size_t write_bfsize;

    AbstractAllocator *absa;
} stream;

stream __openStream(int fd, size_t read_bfsize, size_t write_bfsize);
option __sread(stream str);
option __swrite(stream str, u8 *buff, size_t actual_size);
option __aio_swrite(stream str, u8 *buff, size_t actual_size);
option __aio_sread(stream str);
void __closeStream(stream *str);
#ifdef STREAM_IMPLEMENTATION

option __swrite(stream str, u8 *buff, size_t actual_size){
    if (str.fd < 0) throw("Cannot write to stream, file descriptor incorrect", "Stream.Write.FD.Incorrect", -1);
    if (!buff) throw("Cannot write to stream NULL bytes", "Stream.Data.NULL", 1);
    if (actual_size > str.write_bfsize) throw("Cannot write to stream N amount of bytes, bigger than set at init of stream", "Stream.Data.Size.Incorrect", 2);

    ssize_t act_size = write(str.fd, buff, actual_size);
    if (act_size < 0) throw("Failed write to stream, write size < 0", "Stream.Write.NegativeSize", -2);

    return opt(NULL, act_size, false);
}

stream __openStream(int fd, size_t read_bfsize, size_t write_bfsize){
    AbstractAllocator *absa = td(global.get(".absa"));
    fprintf(stderr, "[glob:%p] opened stream with absa: %p\n", &global, absa);

    return (stream){
        fd, read_bfsize, write_bfsize, absa
    };
}

option __aio_swrite(stream str, u8 *buff, size_t actual_size){
    if (str.fd < 0){
        throw("Invalid stream descriptor", "AIO.Stream.InvalidFD", -1);
    }
    
    ssize_t act_size = write(str.fd, buff, actual_size);

    if (act_size > 0) return opt(NULL, act_size, false);
    if (act_size < 0){
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return opt(NULL, 0, false);
        throw(
            "AIO Stream write error: error while writing (not eagain or ewouldblock)", 
            "AIO.Stream.Write.Failed",
            1
        );
    }

    throw(
        "AIO.Stream.Write Failed to write, socket closed", 
        "AIO.Stream.Write.SockClosed",
        -2
    );
}

option __aio_sread(stream str){
    if (str.fd < 0){
        throw("Invalid stream descriptor", "AIO.Stream.InvalidFD", -1);
    }

    size_t out_size = 0;
    u8*    out_buffer = NULL;
    u8     buffer[MAX_BUFFER_SIZE] = {0};

    ssize_t ar = read(str.fd, buffer, MAX_BUFFER_SIZE);
    if (ar > 0){
        out_buffer = try(str.absa->alloc(str.absa->real, out_size + ar)).data;
        if (out_buffer == NULL){
            throw("AIO Stream read failed, realloc failed","AIO.Stream.Read.Realloc.Failed", -2);
        }

        memcpy(out_buffer + out_size, buffer, ar);
        out_size = ar;
    } else if (ar == 0){ 
        return opt(out_buffer, -out_size, true); // gracefull shutdown from client
    } else if (errno == EAGAIN || errno == EWOULDBLOCK){
        return opt(out_buffer, out_size, true);
    } else if (errno == EINTR){
        return opt(out_buffer, out_size, true);
    } else {
        panic("AIO.Stream.Read.UnreachableHappend");
        throw(        "AIO.Stream.Read Failed to panic, something gone terribly wrong", 
            "AIO.Stream.Fatal.FailedPanic", 
            -1
        );
    }

    return opt(out_buffer, out_size, true);
}

option __sread(stream str){
    if (str.fd < 0) throw("Cannot read from stream, file descriptor incorrect", "Stream.Read.FD.Incorrect", -1);

    fprintf(stderr, "allocating...");
    u8 *obuff = try(str.absa->alloc(str.absa->real, sizeof(u8) * str.read_bfsize)).data;
    if (!obuff) throw("Cannot create temporal buffer for reading from stream, malloc failed", "Stream.Read.Malloc.Failed", -2);

    fprintf(stderr, "reading...");
    ssize_t act_size = read(str.fd, obuff, str.read_bfsize);

    if (act_size < 0){
        fprintf(stderr, "freeing...");
        try(str.absa->free(str.absa->real, obuff));
        throw("Cannot read from stream, read size < 0", "Stream.Read.NegativeSize", -2);
    }

    fprintf(stderr, "just read: %zu\n", act_size);
    return opt(obuff, sizeof(u8) * act_size, true);
}

void __closeStream(stream *str){
    close(str->fd);
}


#endif