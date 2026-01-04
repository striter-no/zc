#pragma once
#include "mds/std/_preincl/base.h"
#include "mds/std/io/stream.h"
#include <mds/std/io/main.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

option __fs_copyPath();
option __fs_movePath();
option __fs_changeOwner();
option __fs_changePermissions();
#ifdef FS_IMPLEMENTATION

option __fs_changePermissions(){
    notimplemented;
}

option __fs_copyPath(){
    notimplemented;
}

option __fs_changeOwner(){
    notimplemented;
}

option __fs_movePath(){
    notimplemented;
}


#endif