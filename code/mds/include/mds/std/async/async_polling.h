#ifndef ASYNC_POLLING_HEADER
#include <mds/std/async/async_api.h>
#include <mds/std/_preincl/defer.h>
#include <mds/std/io/polling.h>
#include <mds/std/containers/table.h>

typedef struct {
    epoller *ptr_eplr;
    kvtable callbacks;

    option (*newptr_callback)(struct epoll_event *ev, epoller *eplr);
} async_ring;

async_ring __async_ring_init(epoller *ptr){
    return (async_ring){
        .ptr_eplr = ptr,
        .callbacks = __kvtable_new()
    };
}

option __async_ring_firstcallback(async_ring *ring, option (*callback)(struct epoll_event *ev, epoller *eplr)){
    if (!ring) throw(
        "Std.Async.Ring: cannot subscribe new callback for ring, ring ptr is NULL", 
        "Std.Async.Ring.FirstSubscribe.Ptr.IsNULL", 
        1
    );

    if (!callback) throw(
        "Std.Async.Ring: cannot subscribe new callback for ring, callback is NULL", 
        "Std.Async.Ring.FirstSubscribe.Callback.IsNULL", 
        2
    );

    ring->newptr_callback = callback;
    return noerropt;
}

option __async_ring_end(async_ring *ring){
    if (!ring) throw(
        "Std.Async.Ring: cannot end ring, ring ptr is NULL", 
        "Std.Async.Ring.End.Ptr.IsNULL", 
        1
    );

    ring->ptr_eplr = NULL;
    try(__kvtable_free(&ring->callbacks));
    return noerropt;
}

option __async_ring_subscribe(async_ring *ring, u32 events, option (*callback)(void *dataptr)){
    if (!ring) throw(
        "Std.Async.Ring: cannot subscribe new callback for ring, ring ptr is NULL", 
        "Std.Async.Ring.Subscribe.Ptr.IsNULL", 
        1
    );

    if (!callback) throw(
        "Std.Async.Ring: cannot subscribe new callback for ring, callback is NULL", 
        "Std.Async.Ring.Subscribe.Callback.IsNULL", 
        2
    );

    try(__kvtable_shallow_set(&ring->callbacks, nv(events), mvar(
        callback, sizeof(void*), false
    )));
    
    return noerropt;
}

option __async_ring_clear(async_ring *ring){
    if (!ring) throw(
        "Std.Async.Ring: cannot clear ring, ring ptr is NULL", 
        "Std.Async.Ring.Clear.Ptr.IsNULL", 
        1
    );

    try(__kvtable_free(&ring->callbacks));
    return noerropt;
}

option __async_ring_iter(void *_ring){
    async_ring *ring = _ring;
    if (!ring) throw(
        "Std.Async.Ring: cannot iter ring, ring ptr is NULL", 
        "Std.Async.Ring.Iter.Ptr.IsNULL", 
        1
    );

    if (ring->callbacks.len == 0) throw(
        "Std.Async.Ring: cannot iter ring, ring has no registered callbacks", 
        "Std.Async.Ring.Iter.NoCallbacks", 
        2
    );

    if (!ring->newptr_callback) throw(
        "Std.Async.Ring: cannot iter ring, ring has no registered callbacks for first interaction (no firstcallback)", 
        "Std.Async.Ring.Iter.NoFirstCallback", 
        3
    );

    var events = try(__epoller_waitev(ring->ptr_eplr, -1));
    // fprintf(stderr, "ring iter: %zu events\n", events.size / sizeof(struct epoll_event));

    coro **awaiting = malloc(sizeof(coro) * events.size);
    defer(^{free(awaiting);});
    
    size_t actual_awaiting = 0;
    for (size_t i = 0; i < events.size; i++){
        var event = (struct epoll_event*)events.data + i * sizeof(struct epoll_event);
        void *ptr = event->data.ptr;
        if (ptr == NULL) {
            ring->newptr_callback(event, ring->ptr_eplr);
            continue;
        }

        for (size_t i = 0; i < ring->callbacks.len; i++){
            u32 events = ring->callbacks.keys.elements[i].size;
            option (*callback)(void*) = ring->callbacks.values.elements[i].data;
            if (!(event->events & events)) 
                continue;
            
            awaiting[actual_awaiting] = try(__std_async_async(callback, ptr, NETWORKING_STACK)).data;
            actual_awaiting++;
            break;
        }
        suspend
    }
    
    suspend
    for (size_t i = 0; i < actual_awaiting; i++){
        try(__std_async_await(awaiting[i]));
        suspend
    }
    return noerropt;
}

#endif
#define ASYNC_POLLING_HEADER
