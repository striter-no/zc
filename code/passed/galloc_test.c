#include <mds/core_impl.h>
#include <mds/modules.h>

$test(galloc){
    void *data = galloc(12);
    gfree(data);
    return noerropt;
}

$test(grealloc){
    void *data = galloc(12);
    void *new_data = grealloc(data, 52);

    gfree(new_data);
    return noerropt;
}

$test(gcalloc){
    void *data = gcalloc(12, 1);
    gfree(data);
    return noerropt;
}
