#pragma once
#include "defs.h"
#include "base/option.h"
#include "base/variable.h"
#include "base/methods.h"

void *__nullpanic(void *allocated){
    if (!allocated) panic("__nullpanic failed");
    return allocated;
}

int __zeropanic(int result){
    if (result == 0) panic("__zeropanic failed");
    return result;
}