#pragma once
#include "mds/std/_preincl/base.h"

typedef struct {
    size_t allocated;
    size_t freed;
} RawAllocator;

RawAllocator __alc_raw_init();
option __alc_raw_allocate(RawAllocator *allc, size_t bytes);
option __alc_raw_free(RawAllocator *allc, void *pointer);
option __alc_raw_reallocate(RawAllocator *allc, void *pointer, size_t new_bytes_size);
option __alc_raw_zeroalloc(RawAllocator *allc, size_t num_elements, size_t one_elem_size);
option __alc_raw_end(RawAllocator *allc);
#ifdef BASIC_IMPLEMENTATION

option __alc_raw_allocate(RawAllocator *allc, size_t bytes){
    if (!allc) throw("RawAllocator: cannot allocate bytes, allc ptr is null", "RawAllocator.Alloc.Ptr.IsNULL", -1);

    // fprintf(stderr, "raw allocating: %zu bytes\n", bytes);
    void *data = malloc(bytes);
    if (!data) throw("RawAllocator: cannot allocate bytes, malloc() failed", "RawAllocator.Alloc.Malloc.Failed", -2);

    allc->allocated++;
    return opt(data, bytes, false);
}

option __alc_raw_free(RawAllocator *allc, void *pointer){
    if (!allc) throw("RawAllocator: cannot free pointer, allc ptr is null", "RawAllocator.Free.Ptr.IsNULL", -1);
    if (!pointer) return noerropt;

    free(pointer);
    allc->freed++;
    return noerropt;
}

option __alc_raw_reallocate(RawAllocator *allc, void *pointer, size_t new_bytes_size){
    if (!allc) throw("RawAllocator: cannot reallocate bytes, allc ptr is null", "RawAllocator.Realloc.Ptr.IsNULL", -1);
    
    
    void *new_ptr = realloc(pointer, new_bytes_size);
    if (!new_ptr) throw(
        "RawAllocator: cannot reallocate bytes, realloc() failed", 
        "RawAllocator.Realloc.Failed", 
        -2
    );

    if (pointer == NULL) allc->allocated++;
    return opt(new_ptr, new_bytes_size, false);
}

RawAllocator __alc_raw_init(){
    return (RawAllocator){
        .allocated = 0,
        .freed = 0
    };
}

option __alc_raw_end(RawAllocator *allc){
    bool incorr = allc->freed != allc->allocated;
    // allc->freed = 0;
    // allc->allocated = 0;

    if (incorr) throw(
        "RawAllocator: end, freed != allocated",
        "RawAllocator.End.AllocAndFreeMismatch",
        1
    );
    return noerropt;
}

option __alc_raw_zeroalloc(RawAllocator *allc, size_t num_elements, size_t one_elem_size){
    if (!allc) throw("RawAllocator: cannot zeroallocate bytes, allc ptr is null", "RawAllocator.ZeroAlloc.Ptr.IsNULL", -1);

    void *data = calloc(num_elements, one_elem_size);
    if (!data) throw("RawAllocator: cannot zeroallocate bytes, malloc() failed", "RawAllocator.ZeroAlloc.Calloc.Failed", -2);

    allc->allocated++;
    return opt(data, num_elements * one_elem_size, false); // <-
}


#endif