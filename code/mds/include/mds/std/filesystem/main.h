#pragma once
#include <mds/genmod.h>
#include "files.h"
#include "directories.h"
#include "general.h"

typedef struct {
    Module _minfo;

    bool (*is_file)(const char *path);
    bool (*is_dir)(const char *path);
    bool (*exists)(const char *path);
    bool (*mkdir)(const char *path, int mode);
    bool (*mkdirs)(const char *path, int mode);

    option (*cwd)();
    option (*change_directory)(const char *path);
    option (*list_files)(Directory *dir, bool recursive);
    option (*list_subdirs)();
    option (*delete_file)();
    option (*delete_dir)();

    option (*freefiles)(array *files);
    option (*fullfile)(File *dummy, const char *mode);
    option (*dummyfile)(const char *filename);
    option (*dummydir)(const char *filename);
    option (*fileopen)(const char *filename, const char *mode);
    option (*fileclose)(File *file);
} std_fs;
std_fs __filesystem;

void __fs_general_setup(){
    __filesystem.is_dir = __fs_is_directory;
    __filesystem.is_file = __fs_is_file;
    __filesystem.exists = __fs_path_exists;
    __filesystem.mkdir = __fs_create_directory;
    __filesystem.mkdirs = __fs_create_directories;
}

void __fs_dirs_setup(){
    __filesystem.cwd = __fs_currentDirectory;
    __filesystem.change_directory = __fs_changeDirectory;
    __filesystem.list_files = __fs_listDirFiles;
    __filesystem.dummydir = __fs_dummydir;
    __filesystem.freefiles = __fs_freeFiles;
}

void __fs_files_setup(){
    __filesystem.fileopen = __fs_fileopen;
    __filesystem.fileclose = __fs_fileclose;
    __filesystem.dummyfile = __fs_dummyfile;
    __filesystem.fullfile = __fs_fullfile;
}

void __fs_setup(){
    __filesystem._minfo = mModuleNew("std.fs");
    __fs_files_setup();
    __fs_dirs_setup();
    __fs_general_setup();
}

#define FS_INJECT_MOD {__filesystem._minfo, &__filesystem}