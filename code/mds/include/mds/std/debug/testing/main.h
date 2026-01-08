#ifndef TESTING_MAIN_HEADER
#include "mds/std/io/main.h"

typedef struct {
    const char *name;
    option (*function)(void);
} Test;

#define $test(name)\
    option __test_##name(void); \
    static void __register_##name(void) __attribute__((constructor)); \
    static void __register_##name(void) { \
        __test_add(#name, __test_##name); \
    } \
    option __test_##name(void)

static Test *__tests_to_pass = NULL;
static size_t __test_count = 0;
void __test_add(const char *name, option (*function)(void)){
    if (__test_count % 10 == 0)
        __tests_to_pass = __nullpanic(realloc(__tests_to_pass, sizeof(Test) * (__test_count + 10)));
    
    __test_count++;
    __tests_to_pass[__test_count - 1] = (Test){name, function};
}

void __run_tests(){
    if (__test_count == 0) {
        fprintf(stderr, "[testing] no tests added, so no tests will be runned\n");
        return;
    }
    fprintf(stderr, "[testing] %zu test will be tested\n", __test_count);
    size_t passed = 0;
    size_t failed = 0;
    for (size_t i = 0; i < __test_count; i++){
        Test *t = &__tests_to_pass[i];
        fprintf(stderr, "%s[%zu/%zu] test: %s\n%s", ANSI.Styles.reset, i + 1, __test_count, t->name, ANSI.Styles.dimmed);
        var result = t->function();
        fprintf(stderr, "%s", ANSI.Styles.reset);
        passed += !is_error(result);
        failed += is_error(result);
        if (is_error(result)){
            __print_stacktrace(stderr);
            fprintf(stderr, "  -> %sFAILED%s (%s:%s)\n", ANSI.HFore.red, ANSI.Styles.reset, gerror(result).type, gerror(result).message);
        } else {
            fprintf(stderr, "  -> %sPASED%s\n", ANSI.HFore.green, ANSI.Styles.reset);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "[testing] %zu passed/%zu failed => %.2f %% passed tests\n", passed, failed, (float)passed/__test_count * 100);
    free(__tests_to_pass);
}

/* example

$test("basic allocator"){
    return noerropt;
}
*/
#endif
#define TESTING_MAIN_HEADER
