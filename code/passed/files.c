#include <mds/core_impl.h>
#include <mds/modules.h>

$test(write){
    var fs = std.fs;
    var io = std.io.term;

    var file = (File*)try(fs.fileopen("./test.txt", "w")).data;
    io.println("File info: %s (permissions: %d)", file->filename, file->permissions);
    try(std.io.sio.swrite(file->selfstr, (u8*)"ZC is cool!", 11));

    fs.fileclose(file);
    return noerropt;
}

$test(open){
    var fs = std.fs;
    var io = std.io.term;
    var snt = std.mem.sentinel;

    var file = (File*)try(fs.fileopen("./test.txt", "r")).data;
    var content = try(std.io.sio.sread(file->selfstr));
    content.clone(&content, try(
        snt.sentinel(content.data, content.size, "\0", 1)
    ));

    io.println("File size: %zu/%zu (%zu)", content.size, file->size);
    io.println("File content: %s", content.data);
    delvar(&content);
    
    fs.fileclose(file);
    return noerropt;
}

$test(current_dir){
    var io = std.io.term;
    var fs = std.fs;
    var cwd = (Directory*)try(fs.cwd()).data;
    
    io.println("Current directory: %s", cwd->path);
    
    free(cwd);
    return noerropt;
}

$test(list_files){
    var io = std.io.term;
    var fs = std.fs;
    var arm = std.array;
    var cwd = (Directory*)try(fs.cwd()).data;

    array *files = try(fs.list_files(cwd, false)).data;
    
    io.println("In directory: %zu files", files->len);
    arm.foreach(files, lambda(variable *fi, size_t inx){
        File *file = fi->data;
        io.println("File: %s (%zu bytes)", file->filename, file->size);
    return 1;});

    arm.dfclean(files, lambda(variable *v){free(v->data);});
    arm.free(files);

    free(files);
    free(cwd);
    return noerropt;
}

$test(list_files_dummy){
    var io = std.io.term;
    var fs = std.fs;
    var arm = std.array;
    var cwd = (Directory*)try(fs.cwd()).data;

    array *files = try(fs.list_files(cwd, false)).data;
    
    io.println("In directory: %zu files", files->len);
    arm.foreach(files, lambda(variable *fi, size_t inx){
        File *file = fi->data;
        io.println("File: %s (%zu bytes)", file->filename, file->size);
    return 1;});

    fs.freefiles(files);
    free(cwd);
    return noerropt;
}

$test(list_files_recursive){
    var io = std.io.term;
    var fs = std.fs;
    var arm = std.array;
    var cwd = (Directory*)try(fs.dummydir("./test")).data;

    array *files = try(fs.list_files(cwd, true)).data;

    arm.foreach(files, lambda(variable *fi, size_t inx){
        File *file = fi->data;
        io.println("File: %s (%zu bytes)", file->filename, file->size);
    return 1;});
    
    fs.freefiles(files);
    free(cwd);
    return noerropt;
}