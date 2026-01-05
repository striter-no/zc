#include "mds/std/_preincl/base/preproc.h"
#include "mds/std/threading/arguments.h"
#include "mds/std/threading/main.h"
#include <mds/modules.h>
#include <mds/core_impl.h>

Mutex *mtx;
i32   counter = 0;

option worker(kvtable args);
option fmain(variable *args, size_t argc){
    var thr = std.threading;
    var io  = std.io.term;
    var mtxm = std.mutex;

    mtx = try(mtxm.create(MUTEX_PLAIN)).data;
    Thread *handles[] = {
        (Thread*)try(thr.spawn(worker, nothrargs.data)).data,
        (Thread*)try(thr.spawn(worker, nothrargs.data)).data
    };

    try(thr.join(*handles[0]));
    try(thr.join(*handles[1]));
    thr.destroy(handles[0]);
    thr.destroy(handles[1]);
    mtxm.destroy(mtx);

    io.println("At end: %d", counter);
    return noerropt;
}

option worker(kvtable args){
    var mtxm = std.mutex;
    for (size_t i = 0; i < 1000000; i++){
        try(mtxm.lock(mtx));
        counter++;
        try(mtxm.unlock(mtx));
    }

    return noerropt;
}
