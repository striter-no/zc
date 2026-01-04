#define FILES_IMPLEMENTATION
#define DIRECTORIES_IMPLEMENTATION
#define FS_IMPLEMENTATION
#define GENERAL_IMPLEMENTATION
#define MDTHREADS_IMPLEMENTATION
#define CONDITIONS_IMPLEMENTATION
#define AWAITER_IMPLEMENTATION
#define ARGUMENTS_IMPLEMENTATION
#define MUTEXES_IMPLEMENTATION
#define OSFUNCS_IMPLEMENTATION
#define SIGNALS_IMPLEMENTATION
#define HASHTABLE_IMPLEMENTATION
#define QUEUE_IMPLEMENTATION
#define TABLE_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define TOKENIZER_IMPLEMENTATION
#define SLICES_IMPLEMENTATION
#define SENTINEL_IMPLEMENTATION
#define GENERALPURPOSE_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#define BASIC_IMPLEMENTATION
#define TCP_SERVER_IMPLEMENTATION
#define TCP_CLIENT_IMPLEMENTATION
#define HASH_IMPLEMENTATION
#define FMT_IMPLEMENTATION
#define STRINGS_IMPLEMENTATION
#define TIMING_IMPLEMENTATION
#define TERM_IMPLEMENTATION
#define COLORS_IMPLEMENTATION
#define POLLING_IMPLEMENTATION
#define STREAM_IMPLEMENTATION
#define RANDOM_IMPLEMENTATION
#define OPTION_IMPLEMENTATION
#define VARIABLE_IMPLEMENTATION
#define GLOBALS_IMPLEMENTATION

#include "../../root.h"
#include "mds/std/_preincl/base.h"
#include "mds/std/_preincl/globals.h"
#include "mds/core.h"

int main(int argc, char *argv[]){
    __global_init();
    __mods_setup();
    
    variable *arguments = malloc(sizeof(variable) * argc);
    if (!arguments) panic("[libc part] failed to allocate memory for arguments");
    for (int i = 0; i < argc; i ++){
        arguments[i].data = argv[i];
        arguments[i].size = 1 + strlen(argv[i]);
    }

    psetup();
    __core_setup();
    option ret = pmain.main(arguments, argc);
    free(arguments);
    free(modules_table);
    __global_deinit();

    if (ret.tag == OPT_ERROR_TYPE){
        fprintf(
            stderr, 
            "\n==============\nprogram exited with error:\n%s: %s\n", 
            ret.variant.err.type,
            ret.variant.err.message
        );
        __print_stacktrace(stderr);
        return ret.variant.err.code;
    }
    
    return 0;
}
