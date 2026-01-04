#include "mds/std/allocators/main.h"
#include <mds/modules.h>

std_io      io;
std_fmt     fmt;
std_array   arrm;
std_kvtable kvt;
std_queue   qu;
std_htable  htm;
std_allc_basic ab;

void load_mods(){
    io   = mInclude(std_io);
    arrm = mInclude(std_array);
    fmt  = mInclude(std_fmt);
    kvt  = mInclude(std_kvtable);
    qu   = mInclude(std_queue);
    htm  = mInclude(std_htable);
    
    ab   = mInclude(std_allc_basic);
}

option fmain(variable *args, size_t argc){
    load_mods();
    
    return noerropt;
}