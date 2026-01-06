#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/containers/array.h"

typedef struct {
    void   *origin;
    void   *data;
    size_t  start_inx;
    size_t  end_inx;
    ssize_t el_size;
    size_t  slice_size;

    bool    copy;
    bool    raw;
    AbstractAllocator *absa;
} Slice;

Slice __std_mem_sliceraw(void *data, size_t start_offset, size_t end_offset);
option __std_mem_ac_sliceraw(void *data, size_t start_offset, size_t end_offset);
Slice __std_mem_slice(void *data, size_t el_size, size_t start_inx, size_t end_inx);
option __std_mem_ac_slice(void *data, size_t el_size, size_t start_inx, size_t end_inx);
option __std_mem_slice_convraw(Slice *raw, size_t el_size);
option __std_mem_slice_fromraw(Slice raw, size_t el_size);
option __std_mem_slice_as_array(Slice slice);
option __std_mem_copy_slice(Slice origin);
option __std_mem_free_slice(Slice *slice);

#ifdef SLICES_IMPLEMENTATION

Slice __std_mem_slice(void *data, size_t el_size, size_t start_inx, size_t end_inx){
    return (Slice){
        .slice_size = end_inx - start_inx,    
        .start_inx = start_inx,    
        .end_inx = end_inx,    
        .el_size = el_size,
        .origin = data,    
        .data = ((u8*)data) + start_inx * el_size,    
        .copy = false,    
        .raw  = false,
        .absa = td(global.get(".absa"))
    };
}

option __std_mem_copy_slice(Slice origin){
    Slice *output = try(origin.absa->alloc(origin.absa->real, sizeof(Slice))).data;
    if (!output) throw(    "Std.Mem.Slice: cannot make copy of the slice, malloc() failed",    "Std.Mem.Slice.SliceCopy.Malloc.Failed",    -1
    );

    void *copied = try(origin.absa->alloc(origin.absa->real, origin.slice_size * (origin.raw ? 1: origin.el_size))).data;
    if (!copied) throw(    "Std.Mem.Slice: cannot make copy of the slice, malloc(2) failed",    "Std.Mem.Slice.SliceCopy.Malloc.2.Failed",    -2
    );
    
    memcpy(copied, origin.data, origin.slice_size * (origin.raw ? 1: origin.el_size));
    *output = (Slice){
        .slice_size = origin.end_inx - origin.start_inx,    
        .start_inx = origin.start_inx,    
        .end_inx = origin.end_inx,    
        .el_size = origin.el_size,
        .origin = origin.data,    
        .data = copied,    
        .copy = true,    
        .raw  = origin.raw,
        .absa = origin.absa
    };

    return opt(output, sizeof(Slice), true);
}

option __std_mem_slice_as_array(Slice slice){
    if (slice.raw) throw(    "Std.Mem.Slice: cannot create array from raw slice",    "Std.Mem.Slice.AsArray.RawSlice",    1
    );

    array *out = try(slice.absa->alloc(slice.absa->real, sizeof(array))).data;
    if (!out) throw(    "Std.Mem.Slice: cannot create array from slice, malloc() failed",    "Std.Mem.Slice.AsArray.Malloc.Failed",    -1
    );

    *out = __array_new();
    try(__array_reserve(out, slice.slice_size));
    for (size_t i = 0; i < slice.slice_size; i++){
        // fprintf(stderr, "saa: %zu\n", i);
        out->elements[i] = mvar(((char*)slice.data) + i * slice.el_size, slice.el_size, false);
    }

    return opt(out, sizeof(array), true);
}

Slice __std_mem_sliceraw(void *data, size_t start_offset, size_t end_offset){
    return (Slice){
        .slice_size = end_offset - start_offset,    
        .start_inx = start_offset,    
        .end_inx = end_offset,    
        .el_size = 0,
        .origin = data,    
        .data = (u8*)data + start_offset,    
        .copy = false,    
        .raw  = true,
        .absa = td(global.get(".absa"))
    };
}

option __std_mem_slice_fromraw(Slice raw, size_t el_size){
    Slice *output = try(raw.absa->alloc(raw.absa->real, sizeof(Slice))).data;
    if (!output) throw(    "Std.Mem.Slice: cannot convert raw slice (fromraw), malloc() failed",    "Std.Mem.Slice.FromRaw.Malloc.Failed",    -1
    );
    try(__std_mem_slice_convraw(output, el_size));
    return opt(output, sizeof(Slice), true);
}

option __std_mem_free_slice(Slice *slice){
    if(!slice) return noerropt;

    if (slice->copy && slice->data) 
        try(slice->absa->free(slice->absa->real, slice->data));
    
    if(slice->copy) 
        try(slice->absa->free(slice->absa->real, slice));
    return noerropt;
}

option __std_mem_ac_slice(void *data, size_t el_size, size_t start_inx, size_t end_inx){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    Slice *output = try(absa->alloc(absa->real, sizeof(Slice))).data;
    if (!output) throw(    "Std.Mem.Slice: cannot make copy slice, malloc() failed",    "Std.Mem.Slice.Copy.Malloc.Failed",    -1
    );

    void *copied = try(absa->alloc(absa->real, (end_inx - start_inx) * el_size)).data;
    if (!copied) throw(    "Std.Mem.Slice: cannot make copy slice, malloc(2) failed",    "Std.Mem.Slice.Copy.Malloc.2.Failed",    -2
    );
    memcpy(copied, data + start_inx * el_size, (end_inx - start_inx) * el_size);
    *output = (Slice){
        .slice_size = end_inx - start_inx,    
        .start_inx = start_inx,    
        .end_inx = end_inx,    
        .el_size = el_size,
        .origin = data,    
        .data = copied,    
        .copy = true,    
        .raw  = false,
        .absa = absa
    };

    return opt(output, sizeof(Slice), true);
}

option __std_mem_ac_sliceraw(void *data, size_t start_offset, size_t end_offset){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    Slice *output = try(absa->alloc(absa->real, sizeof(Slice))).data;
    if (!output) throw(    "Std.Mem.Slice: cannot make raw copy slice, malloc() failed",    "Std.Mem.Slice.CopyRaw.Malloc.Failed",    -1
    );

    void *copied = try(absa->alloc(absa->real, end_offset - start_offset)).data;
    if (!copied) throw(    "Std.Mem.Slice: cannot make raw copy slice, malloc(2) failed",    "Std.Mem.Slice.CopyRaw.Malloc.2.Failed",    -2
    );

    *output = (Slice){
        .slice_size = end_offset - start_offset,    
        .start_inx  = start_offset,    
        .end_inx = end_offset,    
        .el_size = 0,
        .origin = data,    
        .data   = (u8*)copied + start_offset,    
        .copy   = true,    
        .raw    = true,
        .absa = td(global.get(".absa"))
    };

    return opt(output, sizeof(Slice), true);
}

option __std_mem_slice_convraw(Slice *raw, size_t el_size){
    if (el_size == 0) throw(    "Std.Mem.Slice: cannot convert raw slice, el_size cannot be zero",    "Std.Mem.Slice.ConvRaw.ElSize.IsZero",    1
    );

    if (raw->slice_size % el_size != 0) throw(    "Std.Mem.Slice: cannot convert raw slice, slice_size %% el_size != 0",    "Std.Mem.Slice.ConvRaw.InvalidSize",    2
    );

    if (raw->start_inx % el_size != 0) throw(    "Std.Mem.Slice: cannot convert raw slice, start_inx %% el_size != 0",    "Std.Mem.Slice.ConvRaw.InvalidIndex_start",    3
    );

    if (raw->end_inx % el_size != 0) throw(    "Std.Mem.Slice: cannot convert raw slice, end_inx %% el_size != 0",    "Std.Mem.Slice.ConvRaw.InvalidIndex_end",    4
    );

    raw->el_size = el_size;
    raw->raw = false;
    raw->start_inx /= el_size;
    raw->end_inx /= el_size;

    return noerropt;
}


#endif