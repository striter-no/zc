#pragma once
#include "allocators/main.h"
#include "sentinel.h"
#include "slices.h"

typedef struct {
    Module _minfo;

    Slice  (*sliceraw)   (void *data, size_t start_offset, size_t end_offset);
    option (*ac_sliceraw)(void *data, size_t start_offset, size_t end_offset);
    Slice  (*slice)      (void *data, size_t el_size, size_t start_inx, size_t end_inx);
    option (*ac_slice)   (void *data, size_t el_size, size_t start_inx, size_t end_inx);
    option (*slice_convraw) (Slice *raw, size_t el_size);
    option (*slice_fromraw) (Slice  raw, size_t el_size);
    option (*slice_as_array)(Slice  slice);
    option (*copy_slice)(Slice  origin);
    option (*free_slice)(Slice *slice);
} std_mem_slices;
std_mem_slices __std_mem_slices;

typedef struct {
    Module _minfo;

    bool   (*is_sentineled)(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size);
    option (*sentinel)(void *data, size_t data_size, void *sentinel, size_t sentinel_size);
    option (*desentinel)(void *data, size_t data_size, ssize_t el_size, void *sentinel, size_t sentinel_size);
} std_mem_sentinel;
std_mem_sentinel __std_mem_sentinel;

void __slices_setup(){
    __std_mem_slices._minfo = mModuleNew("std.mem.slices");
    __std_mem_slices.sliceraw = __std_mem_sliceraw;
    __std_mem_slices.ac_sliceraw = __std_mem_ac_sliceraw;
    __std_mem_slices.slice = __std_mem_slice;
    __std_mem_slices.ac_slice = __std_mem_ac_slice;
    __std_mem_slices.slice_convraw = __std_mem_slice_convraw;
    __std_mem_slices.slice_fromraw = __std_mem_slice_fromraw;
    __std_mem_slices.slice_as_array = __std_mem_slice_as_array;
    __std_mem_slices.copy_slice = __std_mem_copy_slice;
    __std_mem_slices.free_slice = __std_mem_free_slice;
}

void __sentinel_setup(){
    __std_mem_sentinel._minfo = mModuleNew("std.mem.sentinel");
    __std_mem_sentinel.desentinel = __std_mem_desentinelData;
    __std_mem_sentinel.sentinel = __std_mem_sentinelData;
    __std_mem_sentinel.is_sentineled = __std_mem_isDataSentineled;
}

void __mem_setup(){
    __sentinel_setup();
    __slices_setup();
    __allocators_setup();
}

#define MEM_SENTINEL_INJECT_MOD {__std_mem_sentinel._minfo, &__std_mem_sentinel}
#define MEM_SLICES_INJECT_MOD {__std_mem_slices._minfo, &__std_mem_slices}