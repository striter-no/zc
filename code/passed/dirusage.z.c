#include <mds/core_impl.h>
#include <mds/modules.h>

option fmain(variable *v, size_t c){
    
    String *out = try(talloc(sizeof(String))).data;
    
    var fs = std.fs;
    var tk = std.mem.tokenizer;
    Directory *code = try(fs.dummydir("./code")).data;

    char path[PATH_MAX] = "";
    array *dirs = try(fs.list_files(code, true)).data;
    for (size_t i = 0; i < dirs->len; i++){
        Directory *d = dirs->elements[i].data;
        
        array *samples = try(tk.tokenizeString(d->path, '/')).data;
        
        if (samples->len < 2) continue;
        const char *base = ((Slice*)samples->elements[samples->len - 2].data)->data;

        if (strcmp(base, "subcode") == 0){
            strcpy(path, ((Slice*)samples->elements[2].data)->data);
            std.io.term.println("Had found file: %s", d->path);
            break;
        }
    }

    *out = std.str.fromc(path);
    fs.freeentries(dirs);
    tfree(out);
    
    return noerropt;
}