#ifndef ROOT
#include "mds/std/_preincl/base.h"
#include "mds/std/_preincl/globals.h"
#include "mds/genmod.h"
#include "mds/core.h"

#ifndef main_file
#define main_file main.c
#endif

#define STR(x) #x
#define XSTR(x) STR(x)

#include XSTR(main_file)

struct {
    Module _minfo;
    GeneralPurposeAllocator *allc_gpa;
    BasicAllocator allc_bsc;
    RawAllocator allc_raw;

    AbstractAllocator absa;
    AbstractAllocator absa_bsc;
    AbstractAllocator absa_raw;

    option (*main)(variable *args, size_t argc);
} pmain;

option __testing_main(variable *args, size_t argc){
    __run_tests();
    return noerropt;
}

option pre_setup(){
    pmain.allc_raw = std.mem.allc.raw.init();
    pmain.absa_raw = __mem_std_create_absallc(
        __make_abstract_alloc(__alc_raw_allocate, RawAllocator),
        __make_abstract_zalloc(__alc_raw_zeroalloc, RawAllocator),
        __make_abstract_realloc(__alc_raw_reallocate, RawAllocator),
        __make_abstract_free(__alc_raw_free, RawAllocator),
        &pmain.allc_raw
    );
    try(global.save(".absa.raw", np(&pmain.absa_raw)));

    pmain.allc_gpa = try(std.mem.allc.gpa.init()).data;
    pmain.absa = __mem_std_create_absallc(
        __make_abstract_alloc(__alc_gpa_allocate, GeneralPurposeAllocator),
        __make_abstract_zalloc(__alc_gpa_zeroalloc, GeneralPurposeAllocator),
        __make_abstract_realloc(__alc_gpa_reallocate, GeneralPurposeAllocator),
        __make_abstract_free(__alc_gpa_free, GeneralPurposeAllocator),
        pmain.allc_gpa
    );
    try(global.save(".absa", np(&pmain.absa)));

    pmain.allc_bsc = std.mem.allc.basic.init();
    pmain.absa_bsc = __mem_std_create_absallc(
        __make_abstract_alloc(__alc_basic_allocate, BasicAllocator),
        __make_abstract_zalloc(__alc_basic_zeroalloc, BasicAllocator),
        __make_abstract_realloc(__alc_basic_reallocate, BasicAllocator),
        __make_abstract_free(__alc_basic_free, BasicAllocator),
        &pmain.allc_bsc
    );
    try(global.save(".absa.bsc", np(&pmain.absa_bsc)));
    
    try(__galc_init());
    return noerropt;
}

option post_function(){
    option r;
    
    size_t freezed_freed = pmain.allc_gpa->freed;
    size_t freezed_alced = pmain.allc_gpa->allocated;
    r = std.mem.allc.gpa.end(pmain.allc_gpa);
    #ifdef DEBUG_VERBOSE
    if (is_error(r)) std.io.term.println(
        "\n===============\nend: %s:%s (%zu, %zu)", 
        gerror(r).type, gerror(r).message, 
        freezed_freed, freezed_alced
    );
    #endif

    r = std.mem.allc.basic.end(&pmain.allc_bsc);
    #ifdef DEBUG_VERBOSE
    if (is_error(r)) std.io.term.println("\n===============\nend: %s:%s (%zu, %zu)", gerror(r).type, gerror(r).message, pmain.allc_bsc.freed, pmain.allc_bsc.allocated);
    #endif

    r = std.mem.allc.raw.end(&pmain.allc_raw);
    #ifdef DEBUG_VERBOSE
    if (is_error(r)) std.io.term.println("\n===============\nend: %s:%s (%zu, %zu)", gerror(r).type, gerror(r).message, pmain.allc_raw.freed, pmain.allc_raw.allocated);
    #endif

    __galc_end();
    return noerropt;
}

option __main_wrapped(variable *args, size_t argc){
    try(pre_setup());
    
    #ifndef TESTING
    tryd(fmain(args, argc), post_function);
    #else
    tryd(__testing_main(args, argc), post_function);
    #endif

    try(post_function());
    return noerropt;
}

void psetup(){
    pmain.main = __main_wrapped;
    pmain._minfo = mModuleNew("main");
}
#endif
#define ROOT