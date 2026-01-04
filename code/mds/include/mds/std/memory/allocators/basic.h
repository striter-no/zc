#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/containers/array.h"
#include <mds/std/containers/hashtable.h>

typedef struct {
    array pointers;

    size_t allocated;
    size_t freed;
} BasicAllocator;

BasicAllocator __alc_basic_init();
option __alc_basic_allocate(BasicAllocator *allc, size_t bytes);
option __alc_basic_free(BasicAllocator *allc, void *pointer);
option __alc_basic_reallocate(BasicAllocator *allc, void *pointer, size_t new_bytes_size);
option __alc_basic_zeroalloc(BasicAllocator *allc, size_t num_elements, size_t one_elem_size);
option __alc_basic_end(BasicAllocator *allc);
#ifdef BASIC_IMPLEMENTATION

option __alc_basic_allocate(BasicAllocator *allc, size_t bytes){
    // fprintf(stderr, "--basicallc: allocate\n");

    if (!allc) throw("BasicAllocator: cannot allocate bytes, allc ptr is null", "BasicAllocator.Alloc.Ptr.IsNULL", -1);

    void *data = malloc(bytes);
    if (!data) throw("BasicAllocator: cannot allocate bytes, malloc() failed", "BasicAllocator.Alloc.Malloc.Failed", -2);

    // fprintf(stderr, "basicallc: allocate -> %p\n", data);

    try(__array_shpushback(&allc->pointers, mvar(
        data, bytes, true
    )));

    allc->allocated++;
    // false because we want allocator to manage this pointer, not delvar()
    return opt(data, bytes, false); // <-
}

option __alc_basic_free(BasicAllocator *allc, void *pointer){
    // fprintf(stderr, "--basicallc: free for %p\n", pointer);
    if (!allc) throw("BasicAllocator: cannot free pointer, allc ptr is null", "BasicAllocator.Free.Ptr.IsNULL", -1);
    if (!pointer) return noerropt;

    // fprintf(stderr, "bsc free, pointers: %p %p\n", &allc->pointers, allc->pointers.elements);
    ssize_t index = try(__array_findByPtr(&allc->pointers, pointer)).size;
    if (index == -1 && pointer != NULL) throw(
        "BasicAllocator: cannot free pointers, they are not allocated", 
        "BasicAllocator.Free.Data.NotAllocated", 
        1
    );

    if (index == -1) throw(
        "BasicAllocator: cannot free pointers, they are not allocated in this allocator", 
        "BasicAllocator.Free.Data.NotAllocated_inThisAllocator", 
        2
    );

    // fprintf(stderr, "basicallc: free\n");
    free(pointer);
    try(__array_delat(&allc->pointers, index));
    allc->freed++;
    return noerropt;
}

option __alc_basic_reallocate(BasicAllocator *allc, void *pointer, size_t new_bytes_size){
    if (!allc) throw("BasicAllocator: cannot reallocate bytes, allc ptr is null", "BasicAllocator.Realloc.Ptr.IsNULL", -1);
    
    ssize_t index = try(__array_findByPtr(&allc->pointers, pointer)).size;
    if (index == -1 && pointer != NULL) throw(
        "BasicAllocator: cannot reallocate bytes, they are not allocated", 
        "BasicAllocator.Realloc.Data.NotAllocated", 
        1
    );

    if (index == -1) return __alc_basic_allocate(
        allc, 
        new_bytes_size
    );

    void *new_ptr = realloc(pointer, new_bytes_size);
    if (!new_ptr && index != -1){
        __alc_basic_free(allc, pointer);
    }
    if (!new_ptr) throw(
        "BasicAllocator: cannot reallocate bytes, realloc() failed", 
        "BasicAllocator.Realloc.Failed", 
        -2
    );

    var *ptr = (variable*)(try(__array_refat(&allc->pointers, index)).data);
    ptr->size = new_bytes_size;
    if (new_ptr == pointer) return opt(new_ptr, new_bytes_size, false);
    
    ptr->data = new_ptr;
    return opt(new_ptr, new_bytes_size, false);
}

BasicAllocator __alc_basic_init(){
    return (BasicAllocator){
        .pointers = __array_new_ca(td(global.get(".absa.raw"))),
        .allocated = 0,
        .freed = 0
    };
}

option __alc_basic_end(BasicAllocator *allc){
    bool incorr = allc->freed != allc->allocated;

    size_t freezed_freed = allc->freed;
    size_t freezed_allocated = allc->allocated;
    __array_foreach(&allc->pointers, lambda(variable *vr, size_t inx){
        __alc_basic_free(allc, vr->data);
        return 0;
    });
    __array_free(&allc->pointers);
    allc->freed = freezed_freed;
    allc->allocated = freezed_allocated;

    if (incorr) throw(
        "BasicAllocator: end, freed != allocated",
        "BasicAllocator.End.AllocAndFreeMismatch",
        1
    );
    return noerropt;
}

option __alc_basic_zeroalloc(BasicAllocator *allc, size_t num_elements, size_t one_elem_size){
    if (!allc) throw("BasicAllocator: cannot zeroallocate bytes, allc ptr is null", "BasicAllocator.ZeroAlloc.Ptr.IsNULL", -1);

    void *data = calloc(num_elements, one_elem_size);
    if (!data) throw("BasicAllocator: cannot zeroallocate bytes, malloc() failed", "BasicAllocator.ZeroAlloc.Calloc.Failed", -2);

    try(__array_shpushback(&allc->pointers, mvar(
        data, num_elements * one_elem_size, true
    )));

    allc->allocated++;
    // false because we want allocator to manage this pointer, not delvar()
    return opt(data, num_elements * one_elem_size, false); // <-
}


#endif