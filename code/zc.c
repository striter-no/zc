#include <mds/core_impl.h>
#include <mds/modules.h>

const char *GENERAL_ROOT = 
"#pragma once\n"
"#include <mds/std/_preincl/base.h>\n"
"#include <mds/std/_preincl/globals.h>\n"
"#include <mds/genmod.h>\n"
"#include <mds/core.h>\n"
"#ifndef main_file\n"
"#define main_file main.c\n"
"#endif\n"
"#define STR(x) #x\n"
"#define XSTR(x) STR(x)\n"
"#include XSTR(main_file)\n"
"struct {\n"
"    Module _minfo;\n"
"    option (*main)(variable *args, size_t argc);\n"
"} pmain;\n"
"option __testing_main(variable *args, size_t argc){\n"
"    __run_tests();\n"
"    return noerropt;\n"
"}\n"
"void psetup(){\n"
"    #ifndef TESTING\n"
"    pmain.main = fmain;\n"
"    #else\n"
"    pmain.main = __testing_main;\n"
"    #endif\n"
"    pmain._minfo = mModuleNew(\"main\");\n"
"}\n";

const char *GENERAL_SUBCODE = 
"#define FILES_IMPLEMENTATION\n"
"#define DIRECTORIES_IMPLEMENTATION\n"
"#define FS_IMPLEMENTATION\n"
"#define GENERAL_IMPLEMENTATION\n"
"#define MDTHREADS_IMPLEMENTATION\n"
"#define CONDITIONS_IMPLEMENTATION\n"
"#define AWAITER_IMPLEMENTATION\n"
"#define ARGUMENTS_IMPLEMENTATION\n"
"#define MUTEXES_IMPLEMENTATION\n"
"#define OSFUNCS_IMPLEMENTATION\n"
"#define SIGNALS_IMPLEMENTATION\n"
"#define HASHTABLE_IMPLEMENTATION\n"
"#define QUEUE_IMPLEMENTATION\n"
"#define TABLE_IMPLEMENTATION\n"
"#define ARRAY_IMPLEMENTATION\n"
"#define TOKENIZER_IMPLEMENTATION\n"
"#define SLICES_IMPLEMENTATION\n"
"#define SENTINEL_IMPLEMENTATION\n"
"#define GENERALPURPOSE_IMPLEMENTATION\n"
"#define ARENA_IMPLEMENTATION\n"
"#define BASIC_IMPLEMENTATION\n"
"#define TCP_SERVER_IMPLEMENTATION\n"
"#define TCP_CLIENT_IMPLEMENTATION\n"
"#define HASH_IMPLEMENTATION\n"
"#define FMT_IMPLEMENTATION\n"
"#define STRINGS_IMPLEMENTATION\n"
"#define TIMING_IMPLEMENTATION\n"
"#define TERM_IMPLEMENTATION\n"
"#define COLORS_IMPLEMENTATION\n"
"#define POLLING_IMPLEMENTATION\n"
"#define STREAM_IMPLEMENTATION\n"
"#define RANDOM_IMPLEMENTATION\n"
"#define OPTION_IMPLEMENTATION\n"
"#define VARIABLE_IMPLEMENTATION\n"
"#define GLOBALS_IMPLEMENTATION\n"
"\n"
"#include \"../../root.h\"\n"
"#include <mds/std/_preincl/base.h>\n"
"#include <mds/std/_preincl/globals.h>\n"
"#include <mds/core.h>\n"
"\n"
"int main(int argc, char *argv[]){\n"
"    __global_init();\n"
"    __mods_setup();\n"
"    \n"
"    variable *arguments = malloc(sizeof(variable) * argc);\n"
"    if (!arguments) panic(\"[libc part] failed to allocate memory for arguments\");\n"
"    for (int i = 0; i < argc; i ++){\n"
"        arguments[i].data = argv[i];\n"
"        arguments[i].size = 1 + strlen(argv[i]);\n"
"    }\n"
"\n"
"    psetup();\n"
"    __core_setup();\n"
"    option ret = pmain.main(arguments, argc);\n"
"    free(arguments);\n"
"    free(modules_table);\n"
"    __global_deinit();\n"
"\n"
"    if (ret.tag == OPT_ERROR_TYPE){\n"
"        fprintf(\n"
"            stderr, \n"
"            \"\\n==============\\nprogram exited with error:\n%s: %s\\n\", \n"
"            ret.variant.err.type,\n"
"            ret.variant.err.message\n"
"        );\n"
"        __print_stacktrace(stderr);\n"
"        return ret.variant.err.code;\n"
"    }\n"
"    \n"
"    return 0;\n"
"}\n";


const char *GENERAL_MAIN = 
"#include <mds/core_impl.h>\n"
"#include <mds/modules.h>\n\n"
"option fmain(variable *args, size_t argc){\n"
"\tvar io = std.io.term;\n"
"\tio.println(\"Hello, %s\", \"world!\");\n"
"\treturn noerropt;\n"
"\n}\n";

const char *GENERAL_CLANGD = 
"CompileFlags:\n"
"    Add:\n"
"        - \"-Icode/%s/include\"\n"
"        - \"-I/usr/include\"\n"
"        - \"-Wall\"\n"
"---\n"
"If:\n"
"    PathMatch: [.*\\.c, .*\\.h]\n"
"CompileFlags:\n"
"    Add:\n"
"        - \"-std=c2x\"\n"
"---\n"
"If:\n"
"    PathExclude: [.*\\.c, .*\\.h]\n"
"CompileFlags:\n"
"    Add:\n"
"        - \"-std=c++2x\"\n";

option get_curent_proj(){
    String *out = try(talloc(sizeof(String))).data;
    
    var fs = std.fs;
    Directory *code = try(fs.dummydir("./code")).data;

    array *dirs = try(fs.list_subdirs(code, false)).data;
    for (size_t i = 0; i < dirs->len; i++){
        Directory *d = dirs->elements[i].data;
        std.io.term.println("directory: %s", d->path);
    }
    
    fs.freeentries(dirs);
    return opt(out, sizeof(String), false);
}

option fmain(variable *args, size_t argc){
    var io = std.io.term;
    var fmt = std.fmt;
    var fs = std.fs;
    var balc = (AbstractAllocator*)try(global.get(".absa")).data;
    // io.println("got balc: %p", balc);

    if (argc != 3) {
        io.println("usage zc [run/build/init/test/release] [FILENAME/.../PROJECT NAME]");
        exit(EXIT_FAILURE);
    }

    if (strcmp((char*)args[1].data, "run") == 0){
        var s = try(
            fmt.format("./bin/%s", args[2].data)
        ).data;
        system(s);
    } else if (strcmp((char*)args[1].data, "release") == 0){
        fs.mkdir("./bin", 0700);
        var s = try(
            fmt.format(
                "clang "
                "-std=c2x "
                "-Dmain_file=%s.c "
                "-D_GNU_SOURCE "
                "-DNO_WARNINGS "
                "-O3 -o "
                "./bin/%s "
                "./code/mds/subcode/main.c "
                "-I ./code/mds/include -fblocks -lBlocksRuntime -ldl", 
                args[2].data, args[2].data
            )
        ).data;
        system(s);
        free(s);
    } else if (strcmp((char*)args[1].data, "build") == 0){
        var s = try(
            fmt.format(
                "clang "
                "-std=c2x "
                "-Dmain_file=%s.c "
                "-D_GNU_SOURCE "
                "-fsanitize=undefined,address "
                "-O0 -g -o " //-Wall -Wextra 
                "./bin/%s "
                "./code/mds/subcode/main.c "
                "-I ./code/mds/include -fblocks -lBlocksRuntime -ldl", 
                args[2].data, args[2].data
            )
        ).data;
        system(s);
        try(std.mem.allc.gpa.free(balc->real, s));
    } else if (strcmp((char*)args[1].data, "test") == 0){
        var s = try(
            fmt.format(
                "clang "
                "-std=c2x "
                "-DTESTING "
                "-Dmain_file=%s.c "
                "-D_GNU_SOURCE "
                "-fsanitize=undefined,address "
                "-O0 -gdwarf-4 -o "
                "./bin/%s "
                "./code/mds/subcode/main.c "
                "-I ./code/mds/include -fblocks -lBlocksRuntime", 
                args[2].data, args[2].data
            )
        ).data;
        io.println("[zc] compiling...");
        system(s);
        io.println("[zc] testing started\n");

        s = try(
            fmt.format("./bin/%s", args[2].data)
        ).data;
        system(s);
    } else if (strcmp((char*)args[1].data, "init") == 0){
        io.println("[zc] initializing new project \"%s\"", args[2].data);
        char* st[] = {
            try(fmt.format("code/%s/include/%s", args[2].data, args[2].data)).data,
            try(fmt.format("code/%s/lib", args[2].data)).data,
            try(fmt.format("code/%s/src", args[2].data)).data,
            try(fmt.format("bin")).data,
            try(fmt.format("dev")).data,
            try(fmt.format("./code/%s/subcode")).data
        };

        for (size_t i = 0; i < sizeof(st) / sizeof(st[0]); i++){
            io.println("[%zu/%zu][zc] creating %s...", i + 1, sizeof(st) / sizeof(st[0]), st[i]);
            fs.mkdirs(st[i], 0700);
        }

        io.println("[zc] creating README.md");
        fs.fileclose(try(fs.fileopen("README.md", "w")).data);

        io.println("[zc] creating .gitignore");
        fs.fileclose(try(fs.fileopen(".gitignore", "w")).data);
        
        io.println("[zc] creating code/main.c");
        File *main = try(fs.fileopen("code/main.c", "w")).data;
        std.io.sio.swrite(main->selfstr, (u8*)GENERAL_MAIN, strlen(GENERAL_MAIN));
        fs.fileclose(main);

        io.println("[zc] creating code/%s/subcode/main.c", args[2].data);
        File *submain = try(fs.fileopen(try(fmt.format("code/%s/subcode/main.c", args[2].data)).data, "w")).data;
        std.io.sio.swrite(submain->selfstr, (u8*)GENERAL_SUBCODE, strlen(GENERAL_SUBCODE));
        fs.fileclose(submain);

        io.println("[zc] creating .clangd");
        char *formated_clangd = try(fmt.format(GENERAL_CLANGD, args[2].data)).data;
        File *clangd = try(fs.fileopen(".clangd", "w")).data;
        std.io.sio.swrite(clangd->selfstr, (u8*)formated_clangd, strlen(formated_clangd));
        fs.fileclose(clangd);

        io.println("[zc] creating code/root.h");
        File *root = try(fs.fileopen("code/root.h", "w")).data;
        std.io.sio.swrite(root->selfstr, (u8*)GENERAL_ROOT, strlen(GENERAL_ROOT));
        fs.fileclose(root);

        io.println("[zc] init done");
    } else {
        io.println("invalid param \"%s\"\nusage zc [run/build/init/test/release] [FILENAME/.../PROJECT NAME]", args[1].data);
        exit(EXIT_FAILURE);
    }

    return noerropt;
}
