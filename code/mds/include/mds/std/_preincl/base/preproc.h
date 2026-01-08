#ifndef PREPROC_HEADER
#include "types.h"
#include "../defs.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#ifdef unreachable
#undef unreachable
#endif

#include <unistd.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <dlfcn.h>

static void __print_stacktrace(FILE* output) {
    ;
}

#ifndef __cplusplus
    #if defined(__GNUC__) || defined(__clang__)
        #define UNREACHABLE() __builtin_unreachable()
    #else
        #define UNREACHABLE() abort()
    #endif
#else
    #define UNREACHABLE() std::abort()
#endif

#define unreachable __dwl({ \
    fprintf(stderr, "Unreachable code reached: %s:%d in %s\n", \
            __FILE__, __LINE__, __func__); \
    UNREACHABLE(); \
})

#define __dwl(block) do {block} while(0)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __string_funcsig() TOSTRING(__func__)

__attribute__((noreturn))
inline static void __fn_panic(const char *msg){
    fprintf(
        stderr, "program paniced (%s:%s on %s):\n%s\n", 
        TOSTRING(__FILE__), TOSTRING(__LINE__), __string_funcsig(), 
        msg
    );
    __print_stacktrace(stderr);
    abort();
}
#define panic(msg) __fn_panic(msg); 

#define lambda(...) ^(__VA_ARGS__)

#define try(expr) ({ \
    option _result = (expr); \
    if (_result.tag == OPT_ERROR_TYPE){ \
        /*fprintf(stderr, "[!] failed try() at %s:%d\n", __FILE__, __LINE__);*/\
        return _result;\
    } \
    _result.variant.vrb; \
})

#define tryd(expr, lmbd) ({\
    option _result = (expr); \
    if (_result.tag == OPT_ERROR_TYPE){ \
        /*fprintf(stderr, "[!] failed tryd() at %s:%d\n", __FILE__, __LINE__);*/\
        lmbd();\
        return _result; \
    }\
    _result.variant.vrb; \
})

#define _catch(expr, elseval) ({\
    option _result = (expr);\
    ((_result.tag == OPT_ERROR_TYPE) ? (elseval): (_result.variant.vrb));\
})

#define noerropt opt(NULL, 0, false)

static inline option __fn_throw(const char *msg, const char *type, int code){
    #ifdef DEBUG_VERBOSE
    fprintf(stderr, "throwing...: %s: %s (%d)\n", type, msg, code);
    #endif
    return (option){
        .tag = OPT_ERROR_TYPE,
        .variant = {.err = (error){
            .message = msg,
            .type = type,
            .code = code
        }}
    };
}

#define throw(msg, type, code) return __fn_throw(msg, type, code);
#define thread _Thread_local

#ifndef NO_WARNINGS
#define warning(type, msg) fprintf(stderr, "[%s]: %s\n", type, msg);
#else
#define warning(type, msg) ({;})
#endif

#define notimplemented panic("Method is not implemented");

//#define foreach(arr, item, itype) \
//    size_t i_##__LINE__ = 0; \
//    var _data = (itype)((arr).data); \
//    for (\
//        var *item = &_data[i_##__LINE__]; \
//        i_##__LINE__ < (arr).size; \
//        ++i_##__LINE__\
//    )

#endif
#define PREPROC_HEADER
