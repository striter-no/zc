#pragma once
#include <mds/std/_preincl/base.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mds/std/memory/allocators/abstract.h>

#ifndef ARRAYS_HEAD_STEP
#define ARRAYS_HEAD_STEP 10
#endif

typedef struct {
    variable *elements;

    size_t head_size;
    size_t len;

    AbstractAllocator *absa;
} array;

void __array_foreach(array *arr, int (^expr)(variable *vr, size_t inx));
array __array_new();
array __array_new_ca(AbstractAllocator *absa);
option __array_reserve(array *ptr, size_t len);
option __array_fromc(const void *arr, size_t elsize, size_t n);
option __array_extend_to(array *ptr, size_t len);
option __array_refat(array *ptr, size_t index);
option __array_at(array *ptr, size_t index);
option __array_delat(array *ptr, size_t index);
// retval option<vn<index>>
option __array_index(array *ptr, variable vr);
option __array_findByPtr(array *ptr, void* vr);
// retval option<void>
option __array_pushback(array *ptr, variable vr);
option __array_shpushback(array *ptr, variable vr);
// retval option<void>
option __array_pop(array *ptr);
void __array_dfclean(array *ptr, void (^defer)(variable *vr));
option __array_free(array *ptr);
#ifdef ARRAY_IMPLEMENTATION

array __array_new(){
    return (array){
        .elements = NULL,
        .head_size = 0,
        .len = 0,
        .absa = td(global.get(".absa"))
    };
}

array __array_new_ca(AbstractAllocator *absa){
    return (array){
        .elements = NULL,
        .head_size = 0,
        .len = 0,
        .absa = absa
    };
}

option __array_delat(array *ptr, size_t index){
    if (!ptr) throw("Failed to delete element at given index", "Array.Delete.Ptr.IsNULL", -1);
    if (index >= ptr->len) throw("Failed to delete element at given index", "Array.Del.Index.Overflow", 1);

    delvar(&ptr->elements[index]);
    for (size_t i = index; i < ptr->len - 1; i++){
        try(movevar(
            &ptr->elements[i + 1], 
            &ptr->elements[i]
        ));
    }
    ptr->len--;

    return noerropt;
}

option __array_extend_to(array *ptr, size_t len){
    if (!ptr) throw("Failed to reserve given len of elements in array", "Array.Extend.Ptr.IsNULL", -1);
    if (ptr->len > len) throw("Failed to extend array to given size, size is less than current", "Array.ExtendTo.LenLessCurrent", 1);

    if (len >= ptr->head_size){
        var *newels = try(ptr->absa->realloc(ptr->absa->real, ptr->elements, sizeof(variable) * (ptr->head_size + max(ARRAYS_HEAD_STEP, len)))).data;
        if (!newels) throw("Failed to extend array to given size", "Array.Extend.Realloc.Failed", -2);
        ptr->elements = newels;
        ptr->head_size += max(len, ARRAYS_HEAD_STEP);
    }

    for (size_t i = ptr->len; i < len; i++){
        ptr->elements[i] = mvar(NULL, 0, true);
    }
    
    ptr->len = len;
    return noerropt;
}

void __array_dfclean(array *ptr, void (^defer)(variable *vr)){
    if (!ptr) return;

    for (size_t i = 0; i < ptr->len; i++){
        defer(&ptr->elements[i]);
        delvar(&ptr->elements[i]);
    }
}

option __array_free(array *ptr){
    if (!ptr) return noerropt;

    ptr->absa->free(ptr->absa->real, ptr->elements);
    ptr->elements = NULL;
    ptr->head_size = 0;
    ptr->len = 0;

    return noerropt;
}

option __array_index(array *ptr, variable vr){
    if (!ptr) throw("Failed to index element in array", "Array.Index.Ptr.IsNULL", -1);

    for (size_t i = 0; i < ptr->len; i++) {
        if (vr.size == ptr->elements[i].size && ((vr.data == NULL && vr.size != 0) || memcmp(ptr->elements[i].data, vr.data, vr.size) == 0)){
            return opt(NULL, i, false);
        }
    }

    return opt(NULL, -1, false);
}

option __array_at(array *ptr, size_t index){
    if (!ptr) throw("Failed to get element at given index", "Array.At.Ptr.IsNULL", -1);
    if (index >= ptr->len) throw("Failed to get element at given index", "Array.At.Index.Overflow", 1);
    return opt_var(ptr->elements[index]);
}

option __array_reserve(array *ptr, size_t len){
    if (!ptr) throw("Failed to reserve given len of elements in array", "Array.Reserve.Ptr.IsNULL", -1);
    if (ptr->len != 0) throw("Failed to reserve given len of elements in array, array is not empty", "Array.Reserve.Array.NonZero", 1);

    if (len >= ptr->head_size){
        var *newels = try(ptr->absa->realloc(ptr->absa->real, ptr->elements, sizeof(variable) * (ptr->head_size + max(ARRAYS_HEAD_STEP, len)))).data;
        if (!newels) throw("Failed to reserve given len of elements in array", "Array.Reserver.Realloc.Failed", -2);
        ptr->elements = newels;
        ptr->head_size += max(len, ARRAYS_HEAD_STEP);
    }

    for (size_t i = 0; i < len; i++){
        ptr->elements[i] = mvar(NULL, 0, true);
    }
    
    ptr->len = len;
    return noerropt;
}

option __array_shpushback(array *ptr, variable vr){
    if (!ptr) throw("Failed to push element to the back of an array", "Array.PushBack.Ptr.IsNULL", -1);

    if (ptr->len + 1 >= ptr->head_size){
        var *newels = try(ptr->absa->realloc(ptr->absa->real, ptr->elements, sizeof(variable) * (ptr->head_size + ARRAYS_HEAD_STEP))).data;
        if (!newels) throw("Failed to push element to the back of an array", "Array.PushBack.Realloc.Failed", -2);
        ptr->elements = newels;
        ptr->head_size += ARRAYS_HEAD_STEP;
    }

    ptr->elements[ptr->len] = shcopyvar(vr);
    ptr->len++;

    return noerropt;
}

option __array_findByPtr(array *ptr, void* vr){
    if (!ptr) throw("Failed to index element in array", "Array.Index.Ptr.IsNULL", -1);

    for (size_t i = 0; i < ptr->len; i++) {
        if (vr == ptr->elements[i].data){
            return opt(NULL, i, false);
        }
    }

    return opt(NULL, -1, false);
}

void __array_foreach(array *arr, int (^expr)(variable *vr, size_t inx)){
    if (!arr) return;

    for (size_t i = 0; i < arr->len; i++){
        if (!expr(&arr->elements[i], i))
            break;
    }
}

option __array_fromc(const void *arr, size_t elsize, size_t n){
    if (!arr) throw("Failed to create array from c-array, arr ptr is NULL", "Array.FromC.Ptr.IsNULL", -1);
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    array *new = try(absa->alloc(absa->real, sizeof(array))).data;
    *new = __array_new();
    try(__array_reserve(new, n));
    for (size_t i = 0; i < n; i++){
        new->elements[i] = mvar((u8*)arr + elsize * i, elsize, false);
    }

    return opt(new, sizeof(array), true);
}

option __array_pop(array *ptr){
    if (!ptr) throw("Failed to pop element from the back of an array", "Array.Pop.Ptr.IsNULL", -1);
    if (ptr->len == 0) return noerropt;

    delvar(&ptr->elements[0]);
    ptr->len--;

    return noerropt;
}

option __array_pushback(array *ptr, variable vr){
    if (!ptr) throw("Failed to push element to the back of an array", "Array.PushBack.Ptr.IsNULL", -1);

    if (ptr->len + 1 >= ptr->head_size){
        var *newels = try(ptr->absa->realloc(ptr->absa->real, ptr->elements, sizeof(variable) * (ptr->head_size + ARRAYS_HEAD_STEP))).data;
        if (!newels) throw("Failed to push element to the back of an array", "Array.PushBack.Realloc.Failed", -2);
        ptr->elements = newels;
        ptr->head_size += ARRAYS_HEAD_STEP;
    }

    ptr->elements[ptr->len] = try(copyvar(vr));
    ptr->len++;

    return noerropt;
}

option __array_refat(array *ptr, size_t index){
    if (!ptr) throw("Failed to get element at given index", "Array.RefAt.Ptr.IsNULL", -1);
    if (index >= ptr->len) throw("Failed to get element at given index", "Array.RefAt.Index.Overflow", 1);
    return opt(&ptr->elements[index], sizeof(void*), false);
}


#endif