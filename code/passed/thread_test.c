#include "mds/std/_preincl/base/preproc.h"
#include "mds/std/_preincl/base/types.h"
#include "mds/std/containers/main.h"
#include "mds/std/threading/main.h"
#include "mds/std/threading/mdthreads.h"
#include <mds/modules.h>

std_kvtable skv;
std_io      io;

option thread_pi(kvtable args);
option fmain(variable *args, size_t argc){
    var thr = *(std_threads*)mInclude(std.threading);
    io  = *(std_io*)mInclude(std.io);
    skv = *(std_kvtable*)mInclude(std.kvtable);

    cnst count = 1000000000;
    f64 pi_plus = 0.f;
    f64 pi_minus = 0.f;

    {
        Arguments args;
        args = thrargs({"count", nv(count)}, {"begin", nv(5)}, {"pi", np(&pi_plus)});
        Thread* handle1 = td(thr.spawn(thread_pi, args));

        args = thrargs({"count", nv(count)},{"begin", nv(3)}, {"pi", np(&pi_minus)});
        Thread* handle2 = td(thr.spawn(thread_pi, args));
        
        try(thr.join(*handle1));
        thr.destroy(handle1);

        try(thr.join(*handle2));
        thr.destroy(handle2);
    }

    io.println("Pi: %.8f", 4 + pi_plus - pi_minus);
    return noerropt;
}

option thread_pi(kvtable args){
    cnst end = tv(skv.ats(&args, "count")).size;
    cnst begin = tv(skv.ats(&args, "begin")).size;
    f64 *pi = tv(skv.ats(&args, "pi")).data;

    u64 n = begin; 
    while (n < end) {
        *pi += 4 / (f64)n;
        n += 4;
    }

    return noerropt;
}
