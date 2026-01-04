#pragma once
#include "mds/std/_preincl/base.h"

typedef struct {
    u32 state[4];
} tinymt32;

tinymt32 __math_rnd_tinymt32_create(u32 seed) ;
option __math_rnd_tinymt32_generate(tinymt32 *tmt) ;
option __math_rnd_micro_rand(u8 *state);
option __math_rnd_mulberry32(u32 *state);
option __math_rnd_mwc64x(u64 *state);

#ifdef RANDOM_IMPLEMENTATION

option __math_rnd_mulberry32(u32 *state){
    if (!state) throw(
        "Math.Random mulberry32 failed, state ptr is NULL", 
        "Math.Random.Mulberry32.Ptr.IsNULL", 
        -1
    );

    uint32_t z = (*state += 0x6D2B79F5);
    z = (z ^ (z >> 15)) * (z | 1);
    z ^= z + (z ^ (z >> 7)) * (z | 1);
    return opt(NULL, z ^ (z >> 14), false);
}

tinymt32 __math_rnd_tinymt32_create(u32 seed) {
    tinymt32 tmt;
    tmt.state[0] = seed;
    tmt.state[1] = 0x8f7011ee;
    tmt.state[2] = 0xfc78ff1f;
    tmt.state[3] = 0x3793fdff;

    for (int i = 1; i < 8; i++) {
        tmt.state[i & 3] ^= i + 1812433253 *
            (tmt.state[(i-1) & 3] ^ (tmt.state[(i-1) & 3] >> 30));
    }

    return tmt;
}

option __math_rnd_mwc64x(u64 *state){
    if (!state) throw(
        "Math.Random MWC64X failed, state ptr is NULL", 
        "Math.Random.MWC64X.Ptr.IsNULL", 
        -1
    );

    uint32_t c=(*state)>>32, x=(*state)&0xFFFFFFFF;
    *state = x*((u64)4294883355U) + c;
    return opt(NULL, x^c, 0);
}

option __math_rnd_tinymt32_generate(tinymt32 *tmt) {
    if (!tmt) throw(
        "Math.Random tinymt32_generate failed, tmt ptr is NULL", 
        "Math.Random.TinyMT32.Ptr.IsNULL", 
        -1
    );

    u32 x = (tmt->state[0] & 0x7fffffff) ^ tmt->state[1] ^ tmt->state[2];
    x ^= x << 1;
    tmt->state[0] = tmt->state[1];
    tmt->state[1] = tmt->state[2];
    tmt->state[2] = tmt->state[3] ^ (x >> 1);
    tmt->state[3] = x;
    return opt(NULL, tmt->state[3], false);
}

option __math_rnd_micro_rand(u8 *state){
    if (!state) throw(
        "Math.Random micro_rand failed, state ptr is NULL", 
        "Math.Random.MicroRand.Ptr.IsNULL", 
        -1
    );

    (*state) = 29 * (*state) + 217;
    return opt(NULL, *state, false);
}


#endif