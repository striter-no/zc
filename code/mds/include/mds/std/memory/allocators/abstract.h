#pragma once
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/globals.h>

typedef struct {
    bool can_free;
    bool can_alloc;
    bool can_zalloc;
    bool can_realloc;

    option (^alloc)  (void *allc, size_t bytes);
    option (^zalloc) (void *allc, size_t num_el, size_t el_size);
    option (^realloc)(void *allc, void *ptr, size_t bytes);
    option (^free)   (void *allc, void *ptr);

    void *real;
} AbstractAllocator;

AbstractAllocator __mem_std_create_absallc(
    option (^alloc)  (void *allc, size_t bytes),
    option (^zalloc) (void *allc, size_t num_el, size_t el_size),
    option (^realloc)(void *allc, void *ptr, size_t bytes),
    option (^free)   (void *allc, void *ptr),
    void *real_ptr
){
    return (AbstractAllocator){
        (bool)free, (bool)alloc, (bool)zalloc, (bool)realloc,
        alloc, zalloc, realloc, free, real_ptr
    };
}

option __mem_std_abs_alloc  (AbstractAllocator *allc, size_t bytes){ if(!allc->can_alloc) throw(
    "Mem.Abstract.Alloc: abstract allocator cannot use alloc()", "Mem.Abstract.Alloc.Undefined", 1
); return allc->alloc(allc, bytes); }
option __mem_std_abs_zalloc (AbstractAllocator *allc, size_t num_elements, size_t el_size){ if(!allc->can_zalloc) throw(
    "Mem.Abstract.Alloc: abstract allocator cannot use zalloc()", "Mem.Abstract.ZAlloc.Undefined", 1
); return allc->zalloc(allc, num_elements, el_size); }
option __mem_std_abs_realloc(AbstractAllocator *allc, void *ptr, size_t new_bytes){ if(!allc->can_realloc) throw(
    "Mem.Abstract.Alloc: abstract allocator cannot use realloc()", "Mem.Abstract.Realloc.Undefined", 1
); return allc->realloc(allc, ptr, new_bytes); }
option __mem_std_abs_free   (AbstractAllocator *allc, void *ptr){ if(!allc->can_free) throw(
    "Mem.Abstract.Alloc: abstract allocator cannot use free()", "Mem.Abstract.Free.Undefined", 1
); return allc->free(allc, ptr); }


#define __make_abstract_alloc(func, alctype) lambda(void *allc, size_t bytes){ return func((alctype*)allc, bytes); }
#define __make_abstract_zalloc(func, alctype) lambda(void *allc, size_t num_elements, size_t el_size){ return func((alctype*)allc, num_elements, el_size); }
#define __make_abstract_realloc(func, alctype) lambda(void *allc, void *ptr, size_t new_bytes){ return func((alctype*)allc, ptr, new_bytes); }
#define __make_abstract_free(func, alctype) lambda(void *allc, void *ptr){ return func((alctype*)allc, ptr); }