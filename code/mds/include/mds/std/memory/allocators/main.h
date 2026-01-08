#ifndef ALLOCATORS_MAIN_HEADER
#include <mds/genmod.h>
#include "basic.h"
#include "generalPurpose.h"
#include "arena.h"
#include "raw.h"

typedef struct {
    Module _minfo;

    BasicAllocator (*init)();
    option (*alloc)(BasicAllocator *allc, size_t bytes);
    option (*free)(BasicAllocator *allc, void *pointer);
    option (*realloc)(BasicAllocator *allc, void *pointer, size_t new_bytes_size);
    option (*zeroalloc)(BasicAllocator *allc, size_t num_elements, size_t one_elem_size);
    option (*end)(BasicAllocator *allc);
} std_mem_allc_basic;
std_mem_allc_basic __std_mem_allc_basic_struct;

typedef struct {
    Module _minfo;

    RawAllocator (*init)();
    option (*alloc)(RawAllocator *allc, size_t bytes);
    option (*free)(RawAllocator *allc, void *pointer);
    option (*realloc)(RawAllocator *allc, void *pointer, size_t new_bytes_size);
    option (*zeroalloc)(RawAllocator *allc, size_t num_elements, size_t one_elem_size);
    option (*end)(RawAllocator *allc);
} std_mem_allc_raw;
std_mem_allc_raw __std_mem_allc_raw_struct;

typedef struct {
    Module _minfo;

    option (*init)();
    option (*alloc)(ArenaAllocator *allc, size_t bytes);
    option (*zeroalloc)(ArenaAllocator *allc, size_t num_elements, size_t one_elem_size);
    option (*end)(ArenaAllocator *allc);
} std_mem_allc_arena;
std_mem_allc_arena __std_mem_allc_arena_struct;

typedef struct {
    Module _minfo;

    option (*init)();
    option (*alloc)(GeneralPurposeAllocator *allc, size_t bytes);
    option (*free)(GeneralPurposeAllocator *allc, void *pointer);
    option (*realloc)(GeneralPurposeAllocator *allc, void *pointer, size_t new_bytes_size);
    option (*zeroalloc)(GeneralPurposeAllocator *allc, size_t num_elements, size_t one_elem_size);
    option (*end)(GeneralPurposeAllocator *allc);
} std_mem_allc_gpa;
std_mem_allc_gpa __std_mem_allc_gpa_struct;

void __raw_allc_setup(){
    __std_mem_allc_raw_struct._minfo = mModuleNew("std.mem.allc.raw");
    __std_mem_allc_raw_struct.init = __alc_raw_init;
    __std_mem_allc_raw_struct.alloc = __alc_raw_allocate;
    __std_mem_allc_raw_struct.free = __alc_raw_free;
    __std_mem_allc_raw_struct.realloc = __alc_raw_reallocate;
    __std_mem_allc_raw_struct.zeroalloc = __alc_raw_zeroalloc;
    __std_mem_allc_raw_struct.end = __alc_raw_end;
}

void __basic_allc_setup(){
    __std_mem_allc_basic_struct._minfo = mModuleNew("std.mem.allc.basic");
    __std_mem_allc_basic_struct.init = __alc_basic_init;
    __std_mem_allc_basic_struct.alloc = __alc_basic_allocate;
    __std_mem_allc_basic_struct.free = __alc_basic_free;
    __std_mem_allc_basic_struct.realloc = __alc_basic_reallocate;
    __std_mem_allc_basic_struct.zeroalloc = __alc_basic_zeroalloc;
    __std_mem_allc_basic_struct.end = __alc_basic_end;
}

void __gpa_allc_setup(){
    __std_mem_allc_gpa_struct._minfo = mModuleNew("std.mem.allc.gpa");
    __std_mem_allc_gpa_struct.init = __alc_gpa_init;
    __std_mem_allc_gpa_struct.alloc = __alc_gpa_allocate;
    __std_mem_allc_gpa_struct.free = __alc_gpa_free;
    __std_mem_allc_gpa_struct.realloc = __alc_gpa_reallocate;
    __std_mem_allc_gpa_struct.zeroalloc = __alc_gpa_zeroalloc;
    __std_mem_allc_gpa_struct.end = __alc_gpa_end;
}

void __arena_allc_setup(){
    __std_mem_allc_arena_struct._minfo = mModuleNew("std.mem.allc.arena");
    __std_mem_allc_arena_struct.init = __alc_arena_init;
    __std_mem_allc_arena_struct.alloc = __alc_arena_allocate;
    __std_mem_allc_arena_struct.zeroalloc = __alc_arena_zeroalloc;
    __std_mem_allc_arena_struct.end = __alc_arena_end;
}

void __allocators_setup(){
    __raw_allc_setup();
    __basic_allc_setup();
    __arena_allc_setup();
    __gpa_allc_setup();
}

#define RAW_ALLC_INJECT_MOD {__std_mem_allc_raw_struct._minfo, &__std_mem_allc_raw_struct}
#define BASIC_ALLC_INJECT_MOD {__std_mem_allc_basic_struct._minfo, &__std_mem_allc_basic_struct}
#define ARENA_ALLC_INJECT_MOD {__std_mem_allc_arena_struct._minfo, &__std_mem_allc_arena_struct}
#define GPA_ALLC_INJECT_MOD {__std_mem_allc_gpa_struct._minfo, &__std_mem_allc_gpa_struct}

#endif
#define ALLOCATORS_MAIN_HEADER
