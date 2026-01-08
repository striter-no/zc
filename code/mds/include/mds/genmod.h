#ifndef GENMOD_HEADER

#include <mds/std/_preincl/defs.h>

typedef struct {
    const char *name;
} Module;

// ------------------

Module mModuleNew(
    const char *name
){
    return (Module){
        .name = name
    };
}
#endif
#define GENMOD_HEADER
