#ifndef GENERAL_HEADER
#include "mds/std/_preincl/base.h"
#include "mds/std/io/stream.h"
#include <mds/std/io/main.h>
#include "files.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>

bool __fs_is_file(const char* path) ;
bool __fs_is_directory(const char* path) ;
bool __fs_path_exists(const char* path) ;
bool __fs_create_directory(const char* path, int mode);
bool __fs_create_directories(const char* path, int mode);
#ifdef GENERAL_IMPLEMENTATION

bool __fs_is_directory(const char* path) {
    struct stat buffer;
    if (stat(path, &buffer) != 0) return false;
    return S_ISDIR(buffer.st_mode);
}

bool __fs_path_exists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

bool __fs_create_directories(const char* path, int mode){
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    bool success = false;
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            success = __fs_create_directory(tmp, mode)? true: success;
            *p = '/';
        }
    mkdir(tmp, mode);
    
    return success;
}

bool __fs_create_directory(const char* path, int mode){
    struct stat st = {0};

    if (stat(path, &st) != -1) return false;
    mkdir(path, mode);
    return true;
}

bool __fs_is_file(const char* path) {
    struct stat buffer;
    if (stat(path, &buffer) != 0) return false;
    return S_ISREG(buffer.st_mode);
}


#endif
#endif
#define GENERAL_HEADER
