#ifndef FILES_HEADER
#include "mds/std/_preincl/base.h"
#include "mds/std/_preincl/globals.h"
#include "mds/std/memory/allocators/abstract.h"
#include "mds/std/io/stream.h"
#include <mds/std/io/main.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>

typedef struct {
    char    filename[PATH_MAX];
    u32     permissions;
    stream  selfstr;
    u64     size;
} File;

option __fs_dummyfile(const char *filename);
option __fs_fullfile(File *dummy, const char *mode);
option __fs_fileopen(const char *filename, const char mode[]);
option __fs_fileclose(File *file);

#ifdef FILES_IMPLEMENTATION

option __fs_fileopen(const char *filename, const char mode[]){
    if (!filename) throw(
        "FileSystem failure, cannot open file, filename is NULL",
        "FileSystem.FileOpen.Filename.Incorrect",
        1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    File *ofile = try(absa->alloc(absa->real, sizeof(File))).data;
    if (!ofile) throw(
        "FileSystem failure, cannot open file, malloc failed",
        "FileSystem.FileOpen.Malloc.Failed",
        -1
    ); 
    strcpy(ofile->filename, filename);
    
    FILE *intr = fopen(filename, mode);
    if (!intr) {try(absa->free(absa->real, ofile)); throw(
        "FileSystem failure, cannot open file, fopen failed",
        "FileSystem.FileOpen.Fopen.Failed",
        -3
    );}

    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        try(absa->free(absa->real, ofile));

        throw(
            "FileSystem failure, cannot get permissions for file, stat failed",
            "FileSystem.FileOpen.Stat.Failed",
            -4
        );
    }

    ofile->size = (long long)file_stat.st_size;
    ofile->permissions = file_stat.st_mode & 07777;

    // bool is_to_read = strchr(mode, 'r') != NULL;
    bool is_to_write = strchr(mode, 'w') != NULL;
    ofile->selfstr = __openStream(
        dup(fileno(intr)), 
        is_to_write ? 0: ofile->size, 
        is_to_write ? SIZE_MAX: 0
    );
    fclose(intr);

    return opt(ofile, sizeof(File), true);
}

option __fs_fileclose(File *file){
    if (!file) throw(
        "FileSystem failure, cannot close file, file* ptr is NULL",
        "FileSystem.FileClose.Ptr.IsNULL",
        -1
    );

    __closeStream(&file->selfstr);
    AbstractAllocator *absa = try(global.get(".absa")).data;
    try(absa->free(absa->real, file));

    return noerropt;
}

option __fs_fullfile(File *dummy, const char *mode){
    if (!dummy) throw(
        "FileSystem failure, cannot open file, filename is NULL",
        "FileSystem.FullFile.Filename.Incorrect",
        1
    );

    FILE *intr = fopen(dummy->filename, mode);
    if (!intr) { throw(
        "FileSystem failure, cannot open file, fopen failed",
        "FileSystem.FullFile.Fopen.Failed",
        -1
    );}

    bool is_to_write = strchr(mode, 'w') != NULL;
    dummy->selfstr = __openStream(
        dup(fileno(intr)), 
        is_to_write ? 0: dummy->size, 
        is_to_write ? SIZE_MAX: 0
    );
    fclose(intr);

    return noerropt;
}

option __fs_dummyfile(const char *filename){
    if (!filename) throw(
        "FileSystem failure, cannot open file, filename is NULL",
        "FileSystem.DummyFile.Filename.Incorrect",
        1
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    File *ofile = try(absa->alloc(absa->real, sizeof(File))).data;
    if (!ofile) throw(
        "FileSystem failure, cannot open file, malloc failed",
        "FileSystem.DummyFile.Malloc.Failed",
        -1
    ); 
    strcpy(ofile->filename, filename);

    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        try(absa->free(absa->real, ofile));

        throw(
            "FileSystem failure, cannot get permissions for file, stat failed",
            "FileSystem.DummyFile.Stat.Failed",
            -4
        );
    }

    ofile->size = (long long)file_stat.st_size;
    ofile->permissions = file_stat.st_mode & 07777;
    
    ofile->selfstr = __openStream(0, 0, 0);
    return opt(ofile, sizeof(File), true);
}


#endif
#endif
#define FILES_HEADER
