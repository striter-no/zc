#include <mds/core_impl.h>
#include <mds/modules.h>

$test(arrays){
    var io = std.io.term;
    var arrm = std.array;

    var arr = arrm.new();
    io.println("Array size now: %zu", arr.len);

    try(arrm.pushback(&arr, nv(42)));
    try(arrm.pushback(&arr, nv(67)));
    try(arrm.pushback(&arr, nv(69)));

    io.println("Array size now: %zu", arr.len);
    arrm.foreach(&arr, lambda(variable *vr, size_t in){
        io.println(" in array at %zu: %d", in, vr->size);
        return 0;
    });

    try(arrm.delat(&arr, 0)); // deleting element at 0 index
    io.println("Array size now: %zu", arr.len);
    arrm.foreach(&arr, lambda(variable *vr, size_t in){
        io.println(" in array at %zu: %d", in, vr->size);
        return 0;
    });

    io.println("Changing element at 0 index");
    ((variable*)try(arrm.refat(&arr, 0)).data)->size = 90;
    arrm.foreach(&arr, lambda(variable *vr, size_t in){
        io.println(" in array at %zu: %d", in, vr->size);
        return 0;
    });

    io.println("At index 1: %d", try(arrm.at(&arr, 1)).size);
    io.println("Element 67 is at %d", try(arrm.index(&arr, nv(67))).size);
    arrm.foreach(&arr, lambda(variable *vr, size_t in){
        io.println(" in array at %zu: %d", in, vr->size);
        return 0;
    });

    arrm.free(&arr);
    return noerropt;
}

$test(kvtable){
    var io = std.io.term;
    var kvt = std.kvtable;

    kvtable table = kvt.new();
    io.println("Table size: %zu", table.len);

    var _defer = lambda(){
        kvt.free(&table);
    };

    kvt.set(&table, nv(12), nv(42));
    io.println("Table size: %zu", table.len);
    io.println("At 12 is... %zu", tryd(kvt.at(&table, nv(12)), _defer).size);

    var ref = (variable*)(tryd(kvt.refat(&table, nv(12)), _defer).data);
    ref->size = 90;

    io.println("At 12 is... %zu", tryd(kvt.at(&table, nv(12)), _defer).size);
    io.println("Value 90 has... %zu", tryd(kvt.key_by_val(&table, nv(90)), _defer).size);

    tryd(kvt.delat(&table, nv(12)), _defer);
    io.println("Table size: %zu", table.len);

    kvt.free(&table);
    return noerropt;
}

$test(queue){
    var qu = std.queue;
    var io = std.io.term;

    queue q = qu.new();
    io.println("Queue size: %zu", q.len);
    try(qu.push(&q, nv(12)));
    try(qu.push(&q, nv(57)));
    try(qu.push(&q, nv(30)));
    io.println("Queue size: %zu", q.len);
    io.println("On top is: %zu", try(qu.top(q)).size);
    try(qu.pop(&q));
    io.println("On top is: %zu", try(qu.top(q)).size);
    io.println("Queue size: %zu", q.len);

    qu.free(&q);
    return noerropt;
}

$test(htable){
    var htm = std.htable;
    hashtable* ht = try(htm.new()).data;

    for (int i = 0; i < 2000; i++) {
        char key[20], val[20];
        sprintf(key, "key%d", i);
        sprintf(val, "value%d", i);
        variable k = {key, strlen(key)+1};
        variable v = {val, strlen(val)+1};
        try(htm.set(ht, k, v));
    }
    
    
    htm.free(ht, NULL, NULL);
    free(ht);
    return noerropt;
}