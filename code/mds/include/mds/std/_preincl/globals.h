#ifndef GLOBALS_HEADER
#include "base.h"
// #include <mds/std/threading/arguments.h>
#include <mds/std/memory/allocators/abstract.h>
#include <mds/std/memory/allocators/raw.h>

typedef struct {
    char *key;
    variable vr;
} __glb_ktpair;

typedef struct {
    __glb_ktpair *pairs;
    size_t        len;  
} __glb_pktable;

typedef struct {
    __glb_pktable v;

    option (*get) (char *key);
    option (*copy)(char *key, variable value);
    option (*save)(char *key, variable value);

    AbstractAllocator absa;
    RawAllocator      ralc;
} Globals;
extern Globals global;

__glb_pktable __pktable_init();

option __glb_kt_set(__glb_pktable *t, char *key, variable vr, bool shallow);
option __glb_kt_get(__glb_pktable *t, char *key);
void __glb_kt_free(__glb_pktable *t, void (^_defer)(variable *vr));

option __global_fn_get(char *key);
option __global_fn_copy(char *key, variable value);
option __global_fn_save(char *key, variable value);
// void __global_mset(Arguments args);
void __global_init();
void __global_deinit();

#ifdef GLOBALS_IMPLEMENTATION

Globals global = {0};

__glb_pktable __pktable_init(){
    return (__glb_pktable){
        .pairs = NULL,
        .len = 0,
    };
}

option __glb_kt_set(__glb_pktable *t, char *key, variable vr, bool shallow){
    if (!t || !key) throw(
        "At globals: __glb_kt_set: failed, t ptr is NULL",
        "Globals.__glb.Set.Ptr.IsNULL",
        1
    );
    
    for (size_t i = 0; i < t->len; i++){
        if (strcmp(t->pairs[i].key, key) != 0) continue;
        delvar(&t->pairs[i].vr);
        
        t->pairs[i].vr = (!shallow)? try(copyvar(vr)): shcopyvar(vr); 
        return noerropt;
    }

    t->pairs = try(global.absa.realloc(&global.ralc, t->pairs, sizeof(__glb_ktpair) * (t->len + 1))).data;
    
    t->pairs[t->len].key = try(global.absa.alloc(&global.ralc, strlen(key) + 1)).data;
    strcpy(t->pairs[t->len].key, key);

    t->pairs[t->len].vr = (!shallow)? try(copyvar(vr)): shcopyvar(vr);
    t->len++;
    return noerropt;
}

option __glb_kt_get(__glb_pktable *t, char *key){
    if (!t) throw(
        "At globals: __glb_kt_get: failed, t ptr is NULL",
        "Globals.__glb.Get.Ptr.IsNULL",
        1
    );

    for (size_t i = 0; i < t->len; i++){
        if (strcmp(t->pairs[i].key, key) != 0) continue;
        // fprintf(stderr, "global: accessed %s\n", key);
        return opt_var(t->pairs[i].vr);
    }

    fprintf(stderr, "> prethrow: trying to get global at \"%s\"\n", key);
    throw(
        "KeyValue Table, cannot get element at key, no such key", 
        "KVTable.__glb.Get.NoKey",
        1
    );
}

void __glb_kt_free(__glb_pktable *t, void (^_defer)(variable *vr)){
    if (!t) return;
    for (size_t i = 0; i < t->len; i++){
        global.absa.free(&global.ralc, t->pairs[i].key);
        _defer(&t->pairs[i].vr);
    }
    global.absa.free(&global.ralc, t->pairs);
}

void __global_init(){
    global.v = __pktable_init();
    global.copy = __global_fn_copy;
    global.save = __global_fn_save;
    global.get = __global_fn_get;

    global.ralc = __alc_raw_init();
    global.absa = __mem_std_create_absallc(
        __make_abstract_alloc(__alc_raw_allocate, RawAllocator),
        __make_abstract_zalloc(__alc_raw_zeroalloc, RawAllocator),
        __make_abstract_realloc(__alc_raw_reallocate, RawAllocator),
        __make_abstract_free(__alc_raw_free, RawAllocator),
        &global.ralc
    );
}

option __global_fn_get(char *key){
    return __glb_kt_get(&global.v, key);
}

void __global_deinit(){
    __glb_kt_free(&global.v, ^(variable *val){
        delvar(val);
    });
}

option __global_fn_save(char *key, variable value){
    return __glb_kt_set(&global.v, key, value, true);
}

option __global_fn_copy(char *key, variable value){
    return __glb_kt_set(&global.v, key, value, false);
}

#endif
#endif
#define GLOBALS_HEADER
