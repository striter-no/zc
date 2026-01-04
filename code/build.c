#include <mds/core_impl.h>
#include <mds/modules.h>

#if defined(_WIN32)
const char *platform = "windows"
#elif defined(__linux__)
const char *platform = "linux";
#elif defined(__APPLE__)
const char *platform = "darwin";
#else
const char *platform = "undefined";
#endif

option __call_ccompiler(
    array include_paths,
    array lib_paths,
    const char *sources_path,
    array flags,
    array macros,
    const char *c_main_file,
    const char *output_file
){
    var arm = std.array;
        
    var cmp_str = try(std.fmt.format(
        "clang -std=c2x %s "
    )).data;
    system(cmp_str);
    free(cmp_str);
    return noerropt;
}

option compile(){
    var io = std.io.term;
    io.println("[zc] compiling");
}

option linking(){
    ;
}

option (*stages[])(void) = {
    compile,
    linking
};