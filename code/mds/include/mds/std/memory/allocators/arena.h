#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/containers/array.h"
#include <mds/std/containers/hashtable.h>

#ifndef ARENA_ALLC_STEP
#define ARENA_ALLC_STEP 1024
#endif

typedef struct {
    void* pointers;

    size_t used;
    size_t capacity;
} ArenaAllocator;

option __alc_arena_init();
option __alc_arena_allocate(ArenaAllocator *allc, size_t bytes);
option __alc_arena_zeroalloc(ArenaAllocator *allc, size_t num_elements, size_t one_elem_size);
option __alc_arena_end(ArenaAllocator *allc);
#ifdef ARENA_IMPLEMENTATION

option __alc_arena_init(){
    ArenaAllocator *allc = malloc(sizeof(ArenaAllocator));
    if (!allc) throw("ArenaAllocator failed to init, malloc() failed", "ArenaAllocator.Init.Main.Malloc.Failed", -1);
    
    void *data = malloc(ARENA_ALLC_STEP);
    if (!data) throw("ArenaAllocator failed to init, malloc() failed", "ArenaAllocator.Init.Data.Malloc.Failed", -2);

    *allc = (ArenaAllocator){
        .pointers = data,
        .capacity = ARENA_ALLC_STEP,
        .used = 0
    };

    return opt(allc, sizeof(ArenaAllocator), true);
}

option __alc_arena_allocate(ArenaAllocator *allc, size_t bytes){
    if (!allc) throw("ArenaAllocator: cannot allocate bytes, allc ptr is null", "ArenaAllocator.Alloc.Ptr.IsNULL", -1);
    if (allc->used + bytes > allc->capacity){
        void *newdata = realloc(allc->pointers, allc->capacity + ARENA_ALLC_STEP);
        if (!newdata) throw("ArenaAllocator: cannot allocate bytes, realloc() failed", "ArenaAllocator.NewCapacity.Realloc.Failed", -2);
        allc->pointers = newdata;
        allc->capacity += ARENA_ALLC_STEP;
    }

    void *data = allc->pointers + allc->used;
    allc->used += bytes;
    return opt(data, bytes, false); // <-
}

option __alc_arena_zeroalloc(ArenaAllocator *allc, size_t num_elements, size_t one_elem_size){
    if (!allc) throw("ArenaAllocator: cannot allocate bytes, allc ptr is null", "ArenaAllocator.Alloc.Ptr.IsNULL", -1);
    
    size_t bytes = num_elements * one_elem_size;
    if (allc->used + bytes > allc->capacity){
        void *newdata = realloc(allc->pointers, allc->capacity + ARENA_ALLC_STEP);
        if (!newdata) throw("ArenaAllocator: cannot allocate bytes, realloc() failed", "ArenaAllocator.NewCapacity.Realloc.Failed", -2);
        allc->pointers = newdata;
        allc->capacity += ARENA_ALLC_STEP;
    }

    void *data = allc->pointers + allc->used;
    memset(allc->pointers + allc->used, 0, bytes);

    allc->used += bytes;
    return opt(data, bytes, false); // <-
}

option __alc_arena_end(ArenaAllocator *allc){
    free(allc->pointers);
    allc->pointers = NULL;
    free(allc);
    return noerropt;
}


#endif