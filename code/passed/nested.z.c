#include <mds/core_impl.h>
#include <mds/modules.h>

struct teststr {
    int data;
};

// Эмуляция async_ring - создание нескольких корутин и их await'инг в цикле
option async_worker(void *arg) {
    struct teststr *ptr = arg;
    int worker_id = ptr->data;
    std.io.term.println("Worker %d: started", worker_id);
    
    for (int i = 0; i < 2; i++) {
        std.io.term.println("Worker %d: step %d", worker_id, i);
        suspend;
    }
    
    std.io.term.println("Worker %d: finished", worker_id);
    return noerropt;
}

// Функция, которая эмулирует __async_ring_iter - создает несколько корутин и await'ит их
option ring_simulation(void *arg) {
    std.io.term.println("Ring simulation: start (%d)", *(int*)arg);
    
    // Создаем массив для хранения корутин (как в async_ring)
    coro *coroutines[5];
    struct teststr *worker_ids[5];
    
    // Создаем несколько корутин (как в async_ring_iter)
    for (int i = 0; i < 5; i++) {
        worker_ids[i] = galloc(sizeof(struct teststr));
        worker_ids[i]->data = i;

        coroutines[i] = try(std.async.async(async_worker, worker_ids[i], NETWORKING_STACK)).data;
        std.io.term.println("Created coroutine %d", i);
    }
    
    std.io.term.println("Ring simulation: awaiting all coroutines");
    
    // Await'им все корутины в цикле (как в async_ring)
    for (size_t i = 0; i < 5; i++) {
        std.io.term.println("About to await coroutine %zu", i);
        std.async.await(coroutines[i]);
        std.io.term.println("Finished awaiting coroutine %zu", i);
    }
    
    std.io.term.println("Ring simulation: all coroutines completed");
    return noerropt;
}

option amain(void*){

    int ring = 42;
    for (size_t i = 0; i < 100; i++){
        std.io.term.println("iter: %d\n", i);
        var cr = try(std.async.async(ring_simulation, &ring, NETWORKING_STACK)).data;
        try(std.async.await(cr));
    }

    return noerropt;
}

option fmain(variable *v, size_t c){
    try(std.async.async(amain, NULL, LARGE_STACK));
    
    std.async.run(NULL);
    return noerropt;
}