#pragma once
#include <mds/std/_preincl/globals.h>
#include <mds/std/containers/array.h>
#include "sentinel.h"
#include "slices.h"

option __std_mem_tokenizeMemory(
    void *data, size_t elements_n, size_t elsize, 
    void *delim
){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    array *output = try(absa->alloc(absa->real, sizeof(array))).data;
    *output = __array_new();

    size_t si = 0, ei = 0;
    for (size_t i = 0; i < elements_n; i ++) {
        void *currel = (u8*)data + (i * elsize);
        if (memcmp(currel, delim, elsize) == 0){
            ei++;
            if (ei - si == 0) continue;
            Slice *current_slice = try(absa->alloc(absa->real, sizeof(Slice))).data;
            *current_slice = __std_mem_slice(data, elsize, si, ei);
            __array_shpushback(output, mvar(current_slice, sizeof(Slice), false));
            si = i + 1;
        } else {
            ei = i;
        }
    }

    if (si <= elements_n){
        Slice *current_slice = try(absa->alloc(absa->real, sizeof(Slice))).data;
        *current_slice = __std_mem_slice(data, elsize, si, elements_n - 1);
        __array_shpushback(output, mvar(current_slice, sizeof(Slice), false));
    }
    
    return opt(output, sizeof(array), false);
}

option __std_mem_tokenizeCopyMemory(
    void *data, size_t elements_n, size_t elsize, 
    void *delim
){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    array *output = try(absa->alloc(absa->real, sizeof(array))).data;
    *output = __array_new();

    size_t si = 0, ei = 0;
    for (size_t i = 0; i < elements_n; i ++) {
        void *currel = (u8*)data + (i * elsize);
        if (memcmp(currel, delim, elsize) == 0){
            ei++;
            if (ei - si == 0) continue;
            Slice *current_slice = try(__std_mem_ac_slice(data, elsize, si, ei)).data;
            __array_shpushback(output, mvar(current_slice, sizeof(Slice), false));
            si = i + 1;
        } else {
            ei = i;
        }
    }

    if (si <= elements_n){
        Slice *current_slice = try(__std_mem_ac_slice(data, elsize, si, elements_n - 1)).data;
        __array_shpushback(output, mvar(current_slice, sizeof(Slice), false));
    }
    
    return opt(output, sizeof(array), false);
}

option __std_mem_tokenizeString(
    char *valid_cstring, char delim
){
    char *_delim = malloc(1);
    _delim[0] = delim;

    option r = __std_mem_tokenizeMemory(valid_cstring, strlen(valid_cstring) +1 ,1, _delim);
    free(_delim);
    return r;
}