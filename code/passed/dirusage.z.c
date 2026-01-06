#include <mds/core_impl.h>
#include <mds/modules.h>

option fmain(variable *v, size_t c){
    
    const char *proj_name = "mds";
    String *out = try(talloc(sizeof(String))).data;
    
    var fs = std.fs;
    var tk = std.mem.tokenizer;
    Directory *code = try(fs.dummydir("./code")).data;

    char path[PATH_MAX] = "";
    array *dirs = try(fs.list_subdirs(code, false)).data;
    for (size_t i = 0; i < dirs->len; i++){
        Directory *d = dirs->elements[i].data;
        
        array *samples = try(tk.tokenizeString(d->path, '/')).data;
        const char *base = ((Slice*)samples->elements[samples->len - 1].data)->data;

        if (strcmp(base, proj_name) == 0){
            strcpy(path, d->path);
            std.io.term.println("Had found directory: %s", d->path);
            break;
        }
    }

    char *outpath = try(std.fmt.format(
        "%s/subcode/main.c", path
    )).data;
    
    fs.freeentries(dirs);
    tfree(out);

    std.io.term.println("Final target: %s", outpath);
    return noerropt;
}