#include <mds/core_impl.h>
#include <mds/modules.h>

$test(basic){
    var ab = std.mem.allc.basic;
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

$test(arena){
    var ar = std.mem.allc.arena;
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

$test(gpa){
    var gp = std.mem.allc.gpa;
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