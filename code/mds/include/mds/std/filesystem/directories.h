#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/io/stream.h"
#include <mds/std/io/main.h>
#include <mds/std/containers/array.h>
#include "files.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>

typedef struct {
    char path[PATH_MAX];
    u32  permissions;
} Directory;

option __fs__getDirPermissions(const char *path, u32 *output);
option __fs_currentDirectory();
option __fs_dummydir(const char *path);
option __fs_changeDirectory(const char *path);
// option<array*<File*>>
option __fs_listDirFiles(Directory *zcdir, bool recursive);
option __fs_freeEntries(array *entries);
option __fs_listDirSubdirs(Directory *zcdir, bool recursive);
option __fs_deleteFile();
option __fs_deleteSubDirectory();
#ifdef DIRECTORIES_IMPLEMENTATION

option __fs__getDirPermissions(const char *path, u32 *output){
    struct stat file_stat;
    if (stat(path, &file_stat) == -1) {
        *output = 0;
        throw(
            "FileSystem failure, cannot get permissions for file, stat failed",
            "FileSystem.FileOpen.Stat.Failed",
            -1
        );
    }
    
    *output = file_stat.st_mode & 07777;
    return noerropt;
}

option __fs_deleteFile(){
    return noerropt;
}

option __fs_listDirFiles(Directory *zcdir, bool recursive){
    DIR *dir = opendir(zcdir->path);
    if (dir == NULL) throw(
        "Fs.ListDir.Files: failed to list files in directory, opendir() failed",
        "Fs.ListDir.Files.POSIX.Failure",
        -1
    );
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    array *out = try(absa->alloc(absa->real, sizeof(array))).data;
    if (!out) throw(
        "Fs.ListDir.Files: failed to list files in directory, malloc() failed",
        "Fs.ListDir.Files.Malloc.Failed",
        -2
    );
    
    *out = __array_new();

    struct dirent *entry;
    struct stat file_stat;
    char full_path[PATH_MAX];
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(full_path, sizeof(full_path), "%s/%s", zcdir->path, entry->d_name);
        
        if (stat(full_path, &file_stat) != 0)
            warning("Runtime.FS.Warning", "Failed to get info about entry in directory");

        if (recursive && S_ISDIR(file_stat.st_mode)){
            Directory *subdir = try(__fs_dummydir(full_path)).data;
            array *tmp = try(__fs_listDirFiles(subdir, true)).data;

            for (size_t i = 0; i < tmp->len; i++){
                try(__array_shpushback(
                    out,
                    try(__array_at(tmp, i))
                ));
            }
            
            try(__array_free(tmp));
            try(absa->free(absa->real, tmp));
            try(absa->free(absa->real, subdir));
        }
        
        if (!S_ISREG(file_stat.st_mode)) continue;
        File *file = try(__fs_dummyfile(full_path)).data;
        try(__array_shpushback(
            out, 
            mvar(file, sizeof(File), 
            true
        )));
    }

    closedir(dir);
    return opt(out, sizeof(array), true);
}

option __fs_listDirSubdirs(Directory *zcdir, bool recursive){
    DIR *dir = opendir(zcdir->path);
    if (dir == NULL) throw(
        "Fs.ListDir.Dirs: failed to list dirs in directory, opendir() failed",
        "Fs.ListDir.Dirs.POSIX.Failure",
        -1
    );
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    array *out = try(absa->alloc(absa->real, sizeof(array))).data;
    if (!out) throw(
        "Fs.ListDir.Dirs: failed to list dirs in directory, malloc() failed",
        "Fs.ListDir.Dirs.Malloc.Failed",
        -2
    );
    
    *out = __array_new();

    struct dirent *entry;
    struct stat file_stat;
    char full_path[PATH_MAX];
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(full_path, sizeof(full_path), "%s/%s", zcdir->path, entry->d_name);
        
        if (stat(full_path, &file_stat) != 0)
            warning("Runtime.FS.Warning", "Failed to get info about entry in directory");

        if (S_ISDIR(file_stat.st_mode)){
            Directory *subdir = try(__fs_dummydir(full_path)).data;

            try(__array_shpushback(
                out, 
                mvar(subdir, sizeof(Directory), true)
            ));
            if (!recursive) continue;
            array *tmp = try(__fs_listDirFiles(subdir, true)).data;
            for (size_t i = 0; i < tmp->len; i++){
                try(__array_shpushback(
                    out,
                    try(__array_at(tmp, i))
                ));
            }
            
            try(__array_free(tmp));
            try(absa->free(absa->real, tmp));
            try(absa->free(absa->real, subdir));
        }
    }

    closedir(dir);
    return opt(out, sizeof(array), true);
}

option __fs_dummydir(const char *path){
    if (!path) throw(
        "Fs.Dummy.Dir: failed to get dummy dir, path is NULL",
        "Fs.Dummy.Dir.Path.IsNULL",
        1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    Directory *out = try(absa->alloc(absa->real, sizeof(Directory))).data;
    if (!out) throw(
        "Fs.CWD: failed to get current directory, malloc() failed",
        "Fs.CWD.Malloc.Failed",
        -2
    );
    strcpy(out->path, path);
    try(__fs__getDirPermissions(out->path, &out->permissions));

    return opt(out, sizeof(Directory), true);
}

option __fs_changeDirectory(const char *path){
    if (chdir(path) == 0)
        return noerropt;
    
    throw(
        "Fs.CWD: failed to change current directory, chdir() failed",
        "Fs.CHDIR.POSIX.Failure",
        -1
    );
}

option __fs_deleteSubDirectory(){
    return noerropt;
}

option __fs_currentDirectory(){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) throw(
        "Fs.CWD: failed to get current directory, getcwd() failed",
        "Fs.CWD.POSIX.Failure",
        -1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    Directory *out = try(absa->alloc(absa->real, sizeof(Directory))).data;
    if (!out) throw(
        "Fs.CWD: failed to get current directory, malloc() failed",
        "Fs.CWD.Malloc.Failed",
        -2
    );
    strcpy(out->path, cwd);
    try(__fs__getDirPermissions(out->path, &out->permissions));

    return opt(out, sizeof(Directory), true);
}

option __fs_freeEntries(array *entries){
    if (!entries) return noerropt;
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    __array_dfclean(entries, lambda(variable *v){
        absa->free(absa->real, v->data);
    });
    try(__array_free(entries));

    
    try(absa->free(absa->real, entries));
    return noerropt;
}


#endif