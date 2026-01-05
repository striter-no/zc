#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/containers/array.h"
#include "mds/std/io/main.h"
#include <mds/std/containers/hashtable.h>

typedef struct {
    hashtable *pointers;

    size_t allocated;
    size_t freed;
} GeneralPurposeAllocator;

option __alc_gpa_init();
option __alc_gpa_allocate(GeneralPurposeAllocator *allc, size_t bytes);
option __alc_gpa_free(GeneralPurposeAllocator *allc, void *pointer);
option __alc_gpa_reallocate(GeneralPurposeAllocator *allc, void *pointer, size_t new_bytes_size);
option __alc_gpa_zeroalloc(GeneralPurposeAllocator *allc, size_t num_elements, size_t one_elem_size);
option __alc_gpa_end(GeneralPurposeAllocator *allc);
#ifdef GENERALPURPOSE_IMPLEMENTATION

option __alc_gpa_zeroalloc(GeneralPurposeAllocator *allc, size_t num_elements, size_t one_elem_size){
    if (!allc) throw("GeneralPurposeAllocator: cannot zeroallocate bytes, allc ptr is null", "GeneralPurposeAllocator.ZeroAlloc.Ptr.IsNULL", -1);

    void *data = calloc(num_elements, one_elem_size);
    if (!data) throw("GeneralPurposeAllocator: cannot zeroallocate bytes, malloc() failed", "GeneralPurposeAllocator.ZeroAlloc.Calloc.Failed", -2);

    try(__hsht_set(allc->pointers, mvar(
        data, 0, false
    ), mvar(
        data, num_elements * one_elem_size, false
    )));

    allc->allocated++;
    // false because we want allocator to manage this pointer, not delvar()
    return opt(data, num_elements * one_elem_size, false); // <-
}

option __alc_gpa_end(GeneralPurposeAllocator *allc){
    if (!allc) throw("GeneralPurposeAllocator: cannot end GPA, allc ptr is null", "GeneralPurposeAllocator.End.Ptr.IsNULL", -1);
    bool incorr = allc->freed != allc->allocated;

    __hsht_free(allc->pointers, lambda(variable *vr){
        free(vr->data);
    }, NULL);
    free(allc->pointers);

    // allc->freed = 0;
    // allc->allocated = 0;

    free(allc);
    if (incorr) throw(
        "GeneralPurposeAllocator: end, freed != allocated",
        "GeneralPurposeAllocator.End.AllocAndFreeMismatch",
        1
    );
    return noerropt;
}

option __alc_gpa_allocate(GeneralPurposeAllocator *allc, size_t bytes){
    if (!allc) throw("GeneralPurposeAllocator: cannot allocate bytes, allc ptr is null", "GeneralPurposeAllocator.Alloc.Ptr.IsNULL", -1);

    void *data = malloc(bytes);
    if (!data) throw("GeneralPurposeAllocator: cannot allocate bytes, malloc() failed", "GeneralPurposeAllocator.Alloc.Malloc.Failed", -2);

    fprintf(stderr, "gpa (%p) allocating %zu bytes (ht: %p)\n", allc, bytes, allc->pointers);
    try(__hsht_set(allc->pointers, mvar(
        data, 0, false
    ), mvar(
        data, bytes, false
    )));

    allc->allocated++;
    // false because we want allocator to manage this pointer, not delvar()
    return opt(data, bytes, false); // <-
}

option __alc_gpa_free(GeneralPurposeAllocator *allc, void *pointer){
    if (!allc) throw("GeneralPurposeAllocator: cannot free pointer, allc ptr is null", "GeneralPurposeAllocator.Free.Ptr.IsNULL", -1);
    if (!pointer) return noerropt;

    void *ptr = _catch(
        __hsht_get(allc->pointers, mvar(pointer, 0, false)), 
        mvar(NULL, 0, 0)
    ).data;

    if (ptr == NULL) throw(
        "GeneralPurposeAllocator: cannot free pointers, they are not allocated", 
        "GeneralPurposeAllocator.Free.Data.NotAllocated", 
        1
    );

    if (ptr == NULL && pointer != NULL) throw(
        "GeneralPurposeAllocator: cannot free pointers, they are not allocated in this allocator", 
        "GeneralPurposeAllocator.Free.Data.NotAllocated_inThisAllocator", 
        2
    );

    free(pointer);
    try(__hsht_delete(allc->pointers, mvar(pointer, 0, false)));
    allc->freed++;
    return noerropt;
}

option __alc_gpa_init(){
    // fprintf(stderr, "gpa init\n");
    GeneralPurposeAllocator *allc = malloc(sizeof(GeneralPurposeAllocator));
    if (!allc) throw("GeneralPurposeAllocator: cannot init properly, malloc() failed", "GeneralPurposeAllocator.Init.Main.Malloc.Failed", -1);

    // fprintf(stderr, "gpa hsht init\n");
    hashtable *ht = try(__hsht_new()).data;
    *allc = (GeneralPurposeAllocator){
        .freed = 0,
        .allocated = 0,
        .pointers = ht
    };

    // fprintf(stderr, "gpa end (ht: %p, main: %p)\n", ht, allc);
    return opt(allc, sizeof(GeneralPurposeAllocator), true);
}

option __alc_gpa_reallocate(GeneralPurposeAllocator *allc, void *pointer, size_t new_bytes_size){
    if (!allc) throw("GeneralPurposeAllocator: cannot reallocate bytes, allc ptr is null", "GeneralPurposeAllocator.Realloc.Ptr.IsNULL", -1);
    
    void *ptr = _catch(
        __hsht_get(allc->pointers, mvar(pointer, 0, false)), 
        mvar(NULL, 0, 0)
    ).data;
    if (pointer != NULL && ptr == NULL) throw(
        "GeneralPurposeAllocator: cannot reallocate bytes, they are not allocated", 
        "GeneralPurposeAllocator.Realloc.Data.NotAllocated", 
        1
    );

    if (ptr == NULL) return __alc_gpa_allocate(
        allc, 
        new_bytes_size
    );

    void *new_ptr = realloc(pointer, new_bytes_size);
    if (!new_ptr && ptr != NULL){
        try(__alc_gpa_free(allc, pointer));
    }
    if (!new_ptr) throw(
        "GeneralPurposeAllocator: cannot reallocate bytes, realloc() failed", 
        "GeneralPurposeAllocator.Realloc.Failed", 
        -2
    );

    try(__alc_gpa_free(allc, pointer));
    try(__hsht_set(allc->pointers, mvar(
        new_ptr, 0, false
    ), mvar(
        new_ptr, new_bytes_size, false
    )));

    return opt(new_ptr, new_bytes_size, false);
}


#endif