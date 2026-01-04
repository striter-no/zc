#include "mds/std/_preincl/base/preproc.h"
#include "mds/std/threading/arguments.h"
#include "mds/std/threading/main.h"
#include "mds/std/threading/mutexes.h"
#include <mds/modules.h>

std_kvtable skv;
std_io      io;
std_condt   cndm;
std_awaiter awtm;
std_time    tmm;

Condt   *cnd;
Awaiter *awt;

option consumer(kvtable args);
option producer(kvtable args);
option fmain(variable *args, size_t argc){
    var thr = *(std_threads*)mInclude(std.threading);
    io  =     *(std_io*)     mInclude(std.io);
    skv =     *(std_kvtable*)mInclude(std.kvtable);
    awtm =    *(std_awaiter*)mInclude(std.awaiter);
    tmm =     *(std_time*)   mInclude(std.time);
    cndm =    *(std_condt*)  mInclude(std.condt);

    cnd = try(cndm.create()).data;
    awt = try(awtm.create(cnd, MUTEX_PLAIN)).data;
    cndm.subscribe(cnd);

    Thread *handles[] = {
        (Thread*)try(thr.spawn(producer, nothrargs)).data,
        (Thread*)try(thr.spawn(consumer, nothrargs)).data
    };

    try(thr.join(*handles[0]));
    try(thr.join(*handles[1]));
    thr.destroy(handles[0]);
    thr.destroy(handles[1]);

    return noerropt;
}

option producer(kvtable args){

    while (true) {
        tmm.sleep(tmm.fromSeconds(2));
        io.println("< sending signal...");
        cndm.signal(cnd);
    }

    return noerropt;
}

option consumer(kvtable args){
    while (true){
        io.println("> waiting signal...");
        awtm.wait(awt);
        cndm.subcheck(cnd);
        io.println("> got signal");
    }

    return noerropt;
}
