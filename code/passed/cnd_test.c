#include <mds/core_impl.h>
#include <mds/modules.h>


Condt   *cnd;
Awaiter *awt;

option consumer(kvtable args);
option producer(kvtable args);
option fmain(variable *args, size_t argc){
    var thr = std.threading;
    var io  = std.io.term;
    var skv = std.kvtable;
    var awtm = std.awaiter;
    var tmm = std.time;
    var cndm = std.condt;

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
