#include <mds/core_impl.h>
#include <mds/modules.h>

$test(hello_world){
    return noerropt;
}

$test(fail_test){
    throw(
        "Some error message",
        "Testing.Throw.Test",
        -1
    );
}

$test(out_test){
    var io = std.io.term;
    io.println("Some of output from test");

    return noerropt;
}