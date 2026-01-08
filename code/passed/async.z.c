#include <mds/core_impl.h>
#include <mds/modules.h>

option task1(void *arg) {
    std.io.term.println("Task 1: start");
    
    for (int i = 0; i < 3; i++) {
        std.io.term.println("Task 1: step %d", i);
        suspend;
    }

    std.io.term.println("Task 1: done");
    return noerropt;
}

option task2(void *arg) {
    std.io.term.println("Task 2: start");
    coro *other = (coro*)arg;
    
    std.async.await(other);
    std.io.term.println("Task 2: after await");
    
    for (int i = 0; i < 2; i++) {
        std.io.term.println("Task 2: step %d", i);
        suspend;
    }
    
    std.io.term.println("Task 2: done");
    return noerropt;
}

option fmain(variable *v, size_t c){
    
    var coro1 = try(std.async.async(task1, NULL, REGULAR_STACK));
    // std.async.await(coro1.data);
    try(std.async.async(task2, coro1.data, REGULAR_STACK));

    std.async.run(NULL);
    return noerropt;
}
