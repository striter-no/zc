#ifndef BASE_HEADER
#include "defs.h"
#include "base/option.h"
#include "base/variable.h"
#include "base/methods.h"

void *__nullpanic(void *allocated){
    if (!allocated) panic("__nullpanic triggered");
    return allocated;
}

int __zeropanic(int result){
    if (result == 0) panic("__zeropanic triggered");
    return result;
}

option __errpanic(option opt){
    if (is_error(opt)) panic("__errpanic triggered");
    return opt;
}

#endif
#define BASE_HEADER
