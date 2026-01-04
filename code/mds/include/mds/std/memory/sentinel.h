#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/containers/array.h"

// typedef struct {
// } Sentinel;

bool __std_mem_isDataSentineled(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size);
option __std_mem_sentinelData(void *data, size_t data_size, void *sentinel, size_t sentinel_size);
option __std_mem_desentinelData(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size);

#ifdef SENTINEL_IMPLEMENTATION

bool __std_mem_isDataSentineled(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size){
    if (!data || !sentinel) return false;
    if (data_size < sentinel_size) return false;
    if (el_size != -1 && el_size != sentinel_size) return false;

    size_t step = el_size == -1 ? 1: sentinel_size;
    size_t offset = 0;
    while (offset < data_size - sentinel_size){
        if (memcmp((u8*)data + offset, sentinel, sentinel_size) == 0)
            return true;
        offset += step;
    }
    return false;
}

option __std_mem_sentinelData(void *data, size_t data_size, void *sentinel, size_t sentinel_size){
    if (!sentinel) throw(    "Std.Mem.Sentinel: failed to sentinel data, sentinel is NULL",    "Std.Mem.Sentinel.Failed.Sntl.IsNULL",    1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    void *new_data = try(absa->realloc(absa->real, data, data_size + sentinel_size)).data;
    if (!new_data) throw(    "Std.Mem.Sentinel: failed to sentinel data, realloc() failed",    "Std.Mem.Sentinel.Data.Realloc.Failed",    -1
    );

    memcpy(((u8*)new_data) + data_size, sentinel, sentinel_size);
    return opt(new_data, data_size + sentinel_size, false);
}

option __std_mem_desentinelData(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size){
    if (!data) throw(    "Std.Mem.Sentinel: failed to desentinel data, data is NULL",    "Std.Mem.Sentinel.Desentinel.Data.IsNULL",    1
    );

    if (!sentinel) throw(    "Std.Mem.Sentinel: failed to desentinel data, data is NULL",    "Std.Mem.Sentinel.Desentinel.Data.IsNULL",    1
    );

    if (!__std_mem_isDataSentineled(data, data_size, el_size, sentinel,sentinel_size)) throw(    "Std.Mem.Sentinel: failed to desentinel data, data is NULL",    "Std.Mem.Sentinel.Desentinel.Data.IsNotSentineled",    2
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    void *new_data = try(absa->realloc(absa->real, data, data_size - sentinel_size)).data;
    if (!new_data) throw(    "Std.Mem.Sentinel: failed to sentinel data, realloc() failed",    "Std.Mem.Sentinel.Data.Realloc.Failed",    -1
    );

    return opt(new_data, data_size - sentinel_size, false);
}


#endif