#include <mds/core_impl.h>
#include <mds/modules.h>

option fmain(variable *args, size_t argc){
    var io = std.io.term;
    io.println("Hello, %s!", "world");
    
    return noerropt;
}