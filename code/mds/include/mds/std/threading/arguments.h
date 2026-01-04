#pragma once
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/containers/table.h>

typedef struct {
    const char *name;
    variable    value;
} ArgumentField;

typedef struct {
    size_t count;
    ArgumentField *fields;
    
    AbstractAllocator *absa;
} Arguments;

option create_args(ArgumentField fields[], size_t count);
option free_args(Arguments *args);
kvtable __unwrap_args(Arguments args);
#define thrargs(...) create_args( \
    (ArgumentField[]){__VA_ARGS__}, \
    sizeof((ArgumentField[]){__VA_ARGS__}) / sizeof(ArgumentField) \
)

#define nothrargs create_args(NULL, 0)

#ifdef ARGUMENTS_IMPLEMENTATION

option create_args(ArgumentField fields[], size_t count){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    Arguments *args = try(absa->alloc(absa->real, sizeof(Arguments))).data;
    args->absa = absa;

    if (count == 0){
        args->fields = NULL;
        args->count = 0;
        return opt(args, sizeof(Arguments), true);
    }
    
    args->fields = try(args->absa->alloc(absa->real, count * sizeof(ArgumentField))).data;
    for (size_t i = 0; i < count; i++){
        args->fields[i] = fields[i];
    }
    args->count = count;
    return opt(args, sizeof(Arguments), true);
}

kvtable __unwrap_args(Arguments args){
    kvtable kt = __kvtable_new();
    for (size_t i = 0; i < args.count; i++){
        __kvtable_shallow_sets(&kt, args.fields[i].name, args.fields[i].value);
    }
    return kt;
}

option free_args(Arguments *args){
    if (args->fields){
        for (size_t i = 0; i < args->count; i++){
            delvar(&args->fields[i].value);
        }
        try(args->absa->free(args->absa, args->fields));
        args->fields = NULL;
    }
    args->count = 0;
    return noerropt;
}


#endif