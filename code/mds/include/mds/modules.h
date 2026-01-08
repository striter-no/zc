#ifndef MODULES_HEADER
#include <string.h>

#include "mods.h"
#include "genmod.h"
#include "gallc.h"

#define INJECTION(LC_INJ) {{modules_table[counter++] = (struct ModInfo)LC_INJ; if (counter % 10 == 0 && counter != 0){modules_table = realloc(modules_table, sizeof(struct ModInfo) * counter * 2);}}}

struct ModInfo {
    Module mod;
    void *ptr_to_mod;
};

struct ModInfo *modules_table = NULL;
int LOADED_MODS = 0;

void __mods_setup(){
    __inc_mods_setup();

    int counter = 0;
    modules_table = malloc(sizeof(struct ModInfo) * 10);
    INJECTION(TERMIO_INJECT_MOD);     // std.io.term
    INJECTION(STREAMIO_INJECT_MOD);   // std.io.sio
    INJECTION(EPOLLIO_INJECT_MOD);    // std.io.epoll
    INJECTION(TCP_CLI_INJECT_MOD);    // std.net.tcp.client
    INJECTION(TCP_SERV_INJECT_MOD);   // std.net.tcp.server
    INJECTION(THRD_INJECT_MOD);       // std.threading
    INJECTION(MTX_INJECT_MOD);        // std.mutex
    INJECTION(AWAIT_INJECT_MOD);      // std.awaiter
    INJECTION(CNDT_INJECT_MOD);       // std.condt
    INJECTION(FS_INJECT_MOD);         // std.fs
    INJECTION(FMT_INJECT_MOD);        // std.fmt
    INJECTION(ARR_INJECT_MOD);        // std.array
    INJECTION(KVT_INJECT_MOD);        // std.kvtable
    INJECTION(QUEUE_INJECT_MOD);      // std.queue
    INJECTION(HSHT_INJECT_MOD);       // std.htable
    INJECTION(RAW_ALLC_INJECT_MOD);   // std.mem.allc.raw
    INJECTION(BASIC_ALLC_INJECT_MOD); // std.mem.allc.basic
    INJECTION(ARENA_ALLC_INJECT_MOD); // std.mem.allc.arena
    INJECTION(GPA_ALLC_INJECT_MOD);   // std.mem.allc.gpa
    INJECTION(TIME_INJECT_MOD);       // std.time
    INJECTION(SIGNAL_INJECT_MOD);     // std.signal
    INJECTION(MEM_SENTINEL_INJECT_MOD);   // std.mem.sentinel
    INJECTION(MEM_SLICES_INJECT_MOD);     // std.mem.slices
    INJECTION(MEM_TOKENIZE_INJECT_MOD);   // std.mem.tokenizer
    INJECTION(MATH_RAND_INJECT_MOD);      // std.math.random
    INJECTION(CRYPTO_HASH_INJECT_MOD);    // std.crypto.hash
    INJECTION(STR_INJECT_MOD);            // std.str 
    INJECTION(STD_ASYNC_INJECT_MOD);      // std.async 
    LOADED_MODS = counter;
}

static inline const void *__mf_mInclude(const char *name){
    for (size_t i = 0; i < LOADED_MODS; i++){
        if (strcmp(modules_table[i].mod.name, name) == 0){
            return modules_table[i].ptr_to_mod;
        }
    }

    return NULL;
}

static inline const struct ModInfo *__mf__minfo(const char *name){
    for (size_t i = 0; i < LOADED_MODS; i++){
        if (strcmp(modules_table[i].mod.name, name) == 0){
            return &modules_table[i];
        }
    }

    return NULL;
}

#define mInclude(name) __mf_mInclude(#name)
// #define mInclude(name) ({ \
//     const struct ModInfo* _info = __mf__minfo(#name); \
//     _Static_assert( \
//         strcmp(_info->mod.type_name, TOSTRING(__typeof__(*(void**)_info->ptr_to_mod))) == 0, \
//         "Module type mismatch" \
//     ); \
//     _info->ptr_to_mod; \
// })

// #define USE(mod) ({ \
//     void* _ptr = mInclude(mod); \
//     (*(__typeof__(_ptr)*)_ptr); \
// })
#endif
#define MODULES_HEADER
