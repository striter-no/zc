#include <mds/core_impl.h>
#include <mds/modules.h>

$test(main){
    defer(^(){std.io.term.println("Exited main");});

    {
        defer(^(){std.io.term.println("Exited internal block");});
    }

    return noerropt;
}