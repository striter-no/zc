#ifndef FMT_HEADER
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>

option format(const char fmt[], ...);

#ifdef FMT_IMPLEMENTATION

option format(const char fmt[], ...){
    va_list args;
    va_start(args, fmt);
    
    int required_size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    
    if (required_size < 0) {
        throw(
            "Format failed: invalid format string", 
            "Format.Invalid", 
            1
        );
    }
    
    size_t buffer_size = (size_t)required_size + 1;
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    // fprintf(stderr, "fmt: allocating %zu bytes\n", buffer_size);
    char *buffer = try(absa->alloc(absa->real, buffer_size)).data;
    if (!buffer) {
        throw(
            "Format failed, memory allocation failed", 
            "Format.Malloc.Failed", 
            -2
        );
    }
    
    va_start(args, fmt);
    vsnprintf(buffer, buffer_size, fmt, args);
    va_end(args);
    
    // printf("fmt: %s", buffer);
    return opt(buffer, buffer_size, true);
}


#endif
#endif
#define FMT_HEADER
