#pragma once
#include "modules.h"

struct __core {
    struct {
        std_io_stream sio;
        std_io_term   term;
        std_io_epoll  epoll;
    } io;

    struct {
        struct {
            tcp_cli  client;
            tcp_serv server;
        } tcp;
    } net;
    
    struct {
        struct {
            std_mem_allc_basic basic;
            std_mem_allc_arena arena;
            std_mem_allc_gpa   gpa;
            std_mem_allc_raw   raw;
        } allc;

        std_mem_sentinel sentinel;
        std_mem_slices   slices;
    } mem;

    struct {
        std_math_random random;
    } math;

    struct {
        std_crypto_hash hash;
    } crypto;

    std_threads threading;
    std_mutex   mutex;
    std_awaiter awaiter;
    std_condt   condt;
    std_fs      fs;
    std_fmt     fmt;
    std_str     str;
    std_array   array;
    std_kvtable kvtable;
    std_queue   queue;
    std_htable  htable;
    std_time    time;
};

extern struct __core std;

void __std_setup(){
    std.threading = *(std_threads*)mInclude(std.threading);
    std.mutex = *(std_mutex*)mInclude(std.mutex);
    std.awaiter = *(std_awaiter*)mInclude(std.awaiter);
    std.condt = *(std_condt*)mInclude(std.condt);
    std.fs = *(std_fs*)mInclude(std.fs);
    std.fmt = *(std_fmt*)mInclude(std.fmt);
    std.str = *(std_str*)mInclude(std.str);
    std.array = *(std_array*)mInclude(std.array);
    std.kvtable = *(std_kvtable*)mInclude(std.kvtable);
    std.queue = *(std_queue*)mInclude(std.queue);
    std.htable = *(std_htable*)mInclude(std.htable);
    std.time   = *(std_time*)mInclude(std.time);

    std.io.sio = *(std_io_stream*)mInclude(std.io.sio);
    std.io.term = *(std_io_term*)mInclude(std.io.term);
    std.io.epoll = *(std_io_epoll*)mInclude(std.io.epoll);
    std.net.tcp.client = *(tcp_cli*)mInclude(std.net.tcp.client);
    std.net.tcp.server = *(tcp_serv*)mInclude(std.net.tcp.server);

    std.mem.allc.basic = *(std_mem_allc_basic*)mInclude(std.mem.allc.basic);
    std.mem.allc.arena = *(std_mem_allc_arena*)mInclude(std.mem.allc.arena);
    std.mem.allc.gpa = *(std_mem_allc_gpa*)mInclude(std.mem.allc.gpa);
    std.mem.allc.raw = *(std_mem_allc_raw*)mInclude(std.mem.allc.raw);
    std.mem.sentinel = *(std_mem_sentinel*)mInclude(std.mem.sentinel);
    std.mem.slices = *(std_mem_slices*)mInclude(std.mem.slices);

    std.math.random = *(std_math_random*)mInclude(std.math.random);
	
    std.crypto.hash = *(std_crypto_hash*)mInclude(std.crypto.hash);
}

void __core_setup(){
    __std_setup();
}
