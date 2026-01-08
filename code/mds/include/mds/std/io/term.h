#ifndef TERM_HEADER
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>

static int rstrshr(char *s, char c){
    if (s == NULL) return -1;
    
    for (size_t i = strlen(s) - 1; i >= 0; i--){
        if (s[i] != c) return i;
    }
    return -1;
}

void __print(const char *format, ...);
void __println(const char *format, ...);
option __input(const char *queue);

#ifdef TERM_IMPLEMENTATION

option __input(const char *queue){
    __print(queue);
    
    size_t size = 0;
    char *output = NULL;
    getline(&output, &size, stdin);
    __println("%d", rstrshr(output, ' '));
    output[rstrshr(output, ' ')] = '\0';

    size = rstrshr(output, ' ') + 2;

    AbstractAllocator *absa = try(global.get(".absa")).data;
    char *_absa_output = try(absa->alloc(absa->real, size)).data;
    memcpy(_absa_output, output, size);
    try(absa->free(absa->real, output));

    return opt(_absa_output, size, true);
}

void __print(const char *format, ...){
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void __println(const char *format, ...){
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putc('\n', stdout);
    va_end(args);
}


#endif
#endif
#define TERM_HEADER
