#include <mds/core_impl.h>
#include <mds/modules.h>

$test(basic){
    var gpm = std.mem.allc.gpa;
    GeneralPurposeAllocator* gpa = try(gpm.init()).data;
    
    // Тест 1: Простое выделение/освобождение
    void *p1 = try(gpm.alloc(gpa, 100)).data;
    try(gpm.free(gpa, p1));

    gpm.end(gpa);
    return noerropt;
}

$test(multiple){
    var gpm = std.mem.allc.gpa;
    GeneralPurposeAllocator* gpa = try(gpm.init()).data;

    // Тест 2: Множественные выделения
    void *arr[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = try(gpm.alloc(gpa, 50 * (i + 1))).data;
    }
    for (int i = 0; i < 10; i++) {
        try(gpm.free(gpa, arr[i]));
    }

    gpm.end(gpa);
    return noerropt;
}

$test(realloc){
    var gpm = std.mem.allc.gpa;
    GeneralPurposeAllocator* gpa = try(gpm.init()).data;

    // Тест 3: Realloc
    void *p2 = try(gpm.alloc(gpa, 100)).data;
    void *p3 = try(gpm.realloc(gpa, p2, 200)).data;
    try(gpm.free(gpa, p3));
    
    try(gpm.end(gpa));
    return noerropt;
}
