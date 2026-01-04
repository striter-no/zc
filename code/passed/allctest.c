#include "mds/std/_preincl/base.h"
#include "mds/std/allocators/arena.h"
#include "mds/std/allocators/basic.h"
#include "mds/std/allocators/generalPurpose.h"
#include "mds/std/allocators/main.h"
#include <mds/modules.h>

std_io      io;
std_fmt     fmt;
std_array   arrm;
std_kvtable kvt;
std_queue   qu;
std_htable  htm;
std_allc_basic ab;
std_allc_arena ar;
std_allc_gpa   gp;

void load_mods(){
    io   = *(std_io*)     mInclude(std.io);
    arrm = *(std_array*)  mInclude(std.array);
    fmt  = *(std_fmt*)    mInclude(std.fmt);
    kvt  = *(std_kvtable*)mInclude(std.kvtable);
    qu   = *(std_queue*)  mInclude(std.queue);
    htm  = *(std_htable*) mInclude(std.htable);
    
    ab   = *(std_allc_basic*)mInclude(std.allc.basic);
    ar   = *(std_allc_arena*)mInclude(std.allc.arena);
    gp   = *(std_allc_gpa*)mInclude(std.allc.gpa);
}

option test_basic(){
    struct test {
        int a;
        char b;
    };

    BasicAllocator allc = ab.init();

    struct test *ptr = try(ab.alloc(&allc, sizeof(struct test))).data;
    ptr->a = 0;
    ptr->b = 'a';
    try(ab.free(&allc, ptr));

    try(ab.end(&allc));
    return noerropt;
}

option test_arena(){
    struct test {
        int a;
        char b;
    };

    ArenaAllocator *arena = try(ar.init()).data;

    struct test *ptr = try(ar.alloc(arena, sizeof(struct test))).data;
    ptr->a = 0;
    ptr->b = 'a';

    try(ar.end(arena));
    return noerropt;
}

option test_gpa(){
    struct test {
        int a;
        char b;
    };

    GeneralPurposeAllocator *gpa = try(gp.init()).data;

    struct test *ptr = try(gp.alloc(gpa, sizeof(struct test))).data;
    ptr->a = 0;
    ptr->b = 'a';
    // try(gp.free(gpa, ptr));

    try(gp.end(gpa));
    return noerropt;
}

option fmain(variable *args, size_t argc){
    load_mods();
    
    option result;

    result = test_basic();
    is_error(result) ? 
        io.println("FAILED basic allocator test: %s", gerror(result).message): 
        io.println("PASSED basic allocator test");
    
    result = test_arena();
    is_error(result) ? 
        io.println("FAILED arena allocator test: %s", gerror(result).message): 
        io.println("PASSED arena allocator test");
    
    result = test_gpa();
    is_error(result) ? 
        io.println("FAILED gpa allocator test: %s", gerror(result).message): 
        io.println("PASSED gpa allocator test");

    return noerropt;
}