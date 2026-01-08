#pragma once
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>
#include <ucontext.h>

#ifndef MAX_COROUTINES
#define MAX_COROUTINES 1024
#endif

typedef enum {
    REGULAR_STACK = 1024 * 64,
    NETWORKING_STACK = 1024 * 256,
    LARGE_STACK = 1024 * 512
} STACK_SIZES;

enum { CORO_NEW, CORO_RUNNING, CORO_SUSPENDED, CORO_DONE };

struct coroutine {
    ucontext_t context;
    char *stack;
    void *(*func)(void*);
    void *arg;
    void *result;
    
    struct coroutine *waits_for;
    int state;
    int id;
};
typedef struct coroutine coro;

static ucontext_t main_context;
static struct coroutine *current_coro = NULL;
static struct coroutine *coroutines[MAX_COROUTINES];
static int coro_count = 0;

static void __std_async_raw_coroutine_wrapper(struct coroutine *coro) {
    if (!coro) panic("you cannot use coroutine without running event loop! wrapper()");
    
    coro->state = CORO_RUNNING;
    coro->result = coro->func(coro->arg);
    coro->state = CORO_DONE;
    
    if (setcontext(&main_context) == -1) {
        panic("at coroutine_wrapper, async api: setcontext failed");
    }
}

struct coroutine* __std_async_raw_async(void *(*func)(void*), void* arg, size_t stack_size){
    if (coro_count >= MAX_COROUTINES) return NULL;
    
    struct coroutine *coro = malloc(sizeof(struct coroutine));
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t aligned_stack_size = ((stack_size + page_size - 1) / page_size) * page_size;
    coro->stack = aligned_alloc(16, aligned_stack_size); 

    if (getcontext(&coro->context) == -1) {
        // perror("getcontext");
        free(coro);
        return NULL;
    }
    
    coro->context.uc_stack.ss_sp = coro->stack;
    coro->context.uc_stack.ss_size = aligned_stack_size;
    coro->context.uc_link = &main_context;

    makecontext(&coro->context, (void (*)(void))__std_async_raw_coroutine_wrapper, 1, coro);
    
    coro->func = func;
    coro->arg = arg;
    coro->result = NULL;
    coro->waits_for = NULL;
    coro->state = CORO_NEW;
    coro->id = coro_count;
    
    coroutines[coro_count++] = coro;
    return coro;
}

void __async_suspend() {
    if (!current_coro) return;
    
    current_coro->state = CORO_SUSPENDED;
    swapcontext(&current_coro->context, &main_context);
}

void *__std_async_raw_await(struct coroutine *target) {
    if (!target) panic("you cannot use coroutine, target is NULL");
    
    if (target->state == CORO_DONE)
        return target->result;
    
    if (!current_coro) panic("you cannot use coroutine without running event loop! in await");
    current_coro->state = CORO_SUSPENDED;
    current_coro->waits_for = target;
    
    swapcontext(&current_coro->context, &main_context);
    return target->result;
}

__attribute__((constructor))
void __asyncinit(){
    if (getcontext(&main_context) == -1) {
        panic("at __asyncinit(), getcontext() failed");
    }
}

void __std_async_raw_asyncrun(struct coroutine *main_coro, void (^main_awaiter)(void *retval)) {
    int active_coroutines = coro_count;
    
    while (active_coroutines > 0) {
        active_coroutines = 0;
        
        for (int i = 0; i < coro_count; i++) {
            struct coroutine *coro = coroutines[i];
            
            if (coro->state == CORO_DONE) {
                if (main_coro && coro == main_coro) {
                    main_awaiter(coro->result);
                    // Освобождаем только завершенные корутины
                    free(coro->stack);
                    free(coro);
                    coroutines[i] = NULL; // Помечаем как освобожденную
                }
                continue;
            }
            active_coroutines++;
            
            if (coro->state == CORO_SUSPENDED && coro->waits_for) {
                if (coro->waits_for->state == CORO_DONE) {
                    coro->state = CORO_RUNNING;
                    coro->waits_for = NULL;
                } else {
                    continue;
                }
            }
            
            current_coro = coro;
            if (coro->state == CORO_NEW)
                coro->state = CORO_RUNNING;
            
            if (swapcontext(&main_context, &coro->context) == -1) {
                panic("at asyncrun: failed to swapcontex()");
            }
        }
    }

    // Освобождаем оставшиеся корутины
    for (int i = 0; i < coro_count; i++){
        if (coroutines[i] != NULL) {
            free(coroutines[i]->stack);
            free(coroutines[i]);
        }
    }
    coro_count = 0;
}
// zc version

#define suspend __async_suspend();

struct __intr_async_args {
    option (*func)(void*);
    void *args;
};

void *__std_async_option_wrapper(void *arg){
    struct __intr_async_args *args = (struct __intr_async_args*)arg;
    AbstractAllocator *absa = td(global.get(".absa"));

    option *ret = td(absa->alloc(absa->real, sizeof(option)));
    *ret = args->func(args->args);

    absa->free(absa->real, args);
    return ret;
}

option __std_async_async(
    option (*func)(void*), 
    void* arg, 
    size_t stack_size
){
    if (!func) throw(
        "Std.Async.Async: cannot registrate new coro, function is NULL",
        "Std.Async.Async.Func.IsNULL",
        1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    struct __intr_async_args *args = try(absa->alloc(absa->real, sizeof(struct __intr_async_args))).data;
    args->args = arg;
    args->func = func;

    var coro = __std_async_raw_async(__std_async_option_wrapper, args, stack_size);
    if (!coro){
        throw(
            "Std.Async: failed to create coroutine, getcontext() failed", 
            "Std.Aync.Async.getcontex.Failed", 
            -1
        );
    }
    return opt(coro, sizeof(*coro), false);
}

option __std_async_await(coro *coro_ptr){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    option return_val = {0};
    void *result = __std_async_raw_await(coro_ptr);
    memcpy(&return_val, result, sizeof(option));
    try(absa->free(absa->real, result));

    return return_val;
}

option __std_async_run(coro *main_ptr){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    __block option output;
    var mn_lambda = lambda(void *retval){
        memcpy(&output, retval, sizeof(option));
        absa->free(absa->real, retval);
    };

    __std_async_raw_asyncrun(main_ptr, mn_lambda);
    return output;
}