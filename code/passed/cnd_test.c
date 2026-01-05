#include <mds/core_impl.h>
#include <mds/modules.h>

Condt   *cnd;
Awaiter *awt;

option consumer(kvtable args);
option producer(kvtable args);

$test(conditions){
    var thr = std.threading;
    var awtm = std.awaiter;
    var cndm = std.condt;

    cnd = try(cndm.create()).data;
    awt = try(awtm.create(cnd, MUTEX_PLAIN)).data;
    cndm.subscribe(cnd);

    Thread *handles[] = {
        (Thread*)try(thr.spawn(producer, nothrargs.data)).data,
        (Thread*)try(thr.spawn(consumer, nothrargs.data)).data
    };

    try(thr.join(*handles[0]));
    try(thr.join(*handles[1]));
    thr.destroy(handles[0]);
    thr.destroy(handles[1]);

    return noerropt;
}

option producer(kvtable args){

    for (int i = 0; i < 5; i++) {
        std.time.sleep(std.time.fromSeconds(2));
        std.io.term.println("< sending signal...");
        std.condt.signal(cnd);
    }

    return noerropt;
}

option consumer(kvtable args){
    for (int i = 0; i < 5; i++) {
        std.io.term.println("> waiting signal...");
        std.awaiter.wait(awt);
        std.condt.subcheck(cnd);
        std.io.term.println("> got signal");
    }

    return noerropt;
}
