#pragma once
#include <mds/genmod.h>
#include "async_api.h"

typedef struct {
    Module _minfo;

    option (*async)(option (*func)(void*), void* arg, size_t stack_size);
    option (*await)(coro *ptr);
    void   (*run)(void);
} std_async;
std_async __async_struct;

void __async_setup(){
    __async_struct._minfo = mModuleNew("std.async");
    __async_struct.async = __std_async_async;
    __async_struct.await = __std_async_await;
    __async_struct.run = __std_async_raw_asyncrun;
}

#define STD_ASYNC_INJECT_MOD {__async_struct._minfo, &__async_struct}