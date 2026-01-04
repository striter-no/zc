#include <mds/modules.h>
#include <mds/core_impl.h>

void set_global(){
    global.save("test", nv(12));
}

option use_global(){
    std.io.term.println(
        "global value at \"test\": %zu", 
        try(global.get("test")).size
    );
    return noerropt;
}

option fmain(variable *v, size_t argc){
    set_global();
    try(use_global());
    
    return noerropt;
}