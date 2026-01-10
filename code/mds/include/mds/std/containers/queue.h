#ifndef QUEUE_HEADER
#include <mds/std/_preincl/base.h>
#include "array.h"

typedef struct {
    array data;
    size_t len;

} queue;

queue __queue_new();
option __queue_top(queue q);
option __queue_push(queue *q, variable vr);
option __queue_spush(queue *q, variable vr);
option __queue_spop(queue *q);
option __queue_pop(queue *q);
void __queue_dfclean(queue *q, void (^_defer)(variable *vr));
option __queue_free(queue *q);
#ifdef QUEUE_IMPLEMENTATION

option __queue_pop(queue *q){
    if (!q) throw("Queue failed to push, ptr is NULL", "Queue.Pop.Ptr.IsNULL", -1);
    
    if (q->data.len == 0) return opt_var(nv(
        -1
    ));

    try(__array_delat(&q->data, 0));
    q->len--;

    return opt_var(nv(
        q->len
    ));
}

option __queue_spop(queue *q){
    if (!q) throw("Queue failed to push, ptr is NULL", "Queue.Pop.Ptr.IsNULL", -1);
    
    if (q->data.len == 0) return opt_var(nv(
        -1
    ));

    try(__array_shdelat(&q->data, 0));
    q->len--;

    return opt_var(nv(
        q->len
    ));
}

void __queue_dfclean(queue *q, void (^_defer)(variable *vr)){
    if (!q) return;

    __array_dfclean(&q->data, _defer);
    q->len = 0;
}

option __queue_free(queue *q){
    if (!q) return noerropt;

    try(__array_free(&q->data));
    q->len = 0;
    return noerropt;
}

queue __queue_new(){
    return (queue){
        .data = __array_new(),
        .len = 0
    };
}

option __queue_top(queue q){
    // if (!q) throw("Queue failed to get top element, ptr is NULL", "Queue.Top.Ptr.IsNULL", -1);
    if (q.len == 0) throw("Queue failed to get top element, queue is empty", "Queue.Top.Queue.Empty", 1);
    return opt_var(q.data.elements[0]);
}

option __queue_push(queue *q, variable vr){
    if (!q) throw("Queue failed to push, ptr is NULL", "Queue.Push.Ptr.IsNULL", -1);
    
    try(__array_pushback(&q->data, vr));
    q->len++;
    return noerropt;
}

option __queue_spush(queue *q, variable vr){
    if (!q) throw("Queue failed to push, ptr is NULL", "Queue.Push.Ptr.IsNULL", -1);
    
    try(__array_shpushback(&q->data, vr));
    q->len++;
    return noerropt;
}


#endif
#endif
#define QUEUE_HEADER
