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

typedef struct {
    void (*func_deferrerr)(void);
    void (^block_deferrer)(void);
} __dfobject;

__dfobject *__make_base_deferer(
    void (*func_deferrerr)(void),
    void (^block_deferrer)(void)
){
    __dfobject* df = __nullpanic(malloc(sizeof(__dfobject)));
    df->block_deferrer = block_deferrer;
    df->func_deferrerr = func_deferrerr;
    return df;
}

void __base_deffering(__dfobject **dfobject){
    __dfobject *obj = *dfobject;
    if (obj->block_deferrer){
        obj->block_deferrer();
    } else if (obj->func_deferrerr){
        obj->func_deferrerr();
    }
    free(obj);
}

#define IS_BLOCK(x) _Generic((x), \
    void (^)(void): 1, \
    int (^)(void): 1, \
    default: 0)

#define defer(func)\
    __attribute__((cleanup(__base_deffering)))\
    __dfobject *__LINE__##dfobject = IS_BLOCK(func) ? \
                                        __make_base_deferer(NULL, (void (^)(void))func): \
                                        __make_base_deferer((void (*)(void))func, NULL);\
    __LINE__##dfobject; /*just to get rid of unused-variable warning*/
#endif
#define BASE_HEADER
