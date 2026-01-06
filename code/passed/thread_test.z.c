#include <mds/core_impl.h>
#include <mds/modules.h>

option thread_pi(kvtable args);
option fmain(variable *args, size_t argc){
    var thr = std.threading;
    var io  = std.io.term;
    var skv = std.kvtable;

    cnst count = 1000000000;
    f64 pi_plus = 0.f;
    f64 pi_minus = 0.f;

    {
        Arguments *args;
        args = try(thrargs({"count", nv(count)}, {"begin", nv(5)}, {"pi", np(&pi_plus)})).data;
        Thread* handle1 = td(thr.spawn(thread_pi, args));

        args = try(thrargs({"count", nv(count)},{"begin", nv(3)}, {"pi", np(&pi_minus)})).data;
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
    var skv = std.kvtable;

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
