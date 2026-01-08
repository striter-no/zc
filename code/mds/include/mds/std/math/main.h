#ifndef MATH_MAIN_HEADER
#include <mds/genmod.h>
#include "random.h"

typedef struct {
    Module _minfo;

    tinymt32 (*tinymt32_create)  (u32       seed);
    option   (*tinymt32_generate)(tinymt32 *tmt);
    option   (*micro_rand)       (u8       *state);
    option   (*mulberry32)       (u32      *state);
    option   (*mwc64x)           (uint64_t *state);
} std_math_random;
std_math_random __std_math_random_struct;

void __math_random_setup(){
    __std_math_random_struct._minfo = mModuleNew("std.math.random");
    __std_math_random_struct.tinymt32_create = __math_rnd_tinymt32_create;
    __std_math_random_struct.micro_rand = __math_rnd_micro_rand;
    __std_math_random_struct.tinymt32_generate = __math_rnd_tinymt32_generate;
    __std_math_random_struct.mulberry32 = __math_rnd_mulberry32;
    __std_math_random_struct.mwc64x = __math_rnd_mwc64x;
}

void __math_setup(){
    __math_random_setup();
}

#define MATH_RAND_INJECT_MOD {__std_math_random_struct._minfo, &__std_math_random_struct}

#endif
#define MATH_MAIN_HEADER
