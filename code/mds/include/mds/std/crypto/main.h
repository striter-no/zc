#ifndef CRYPTO_MAIN_HEADER
#include <mds/genmod.h>
#include "hash.h"

typedef struct {
    Module _minfo;

    option (*fletch32)(const u16 *data, size_t len);
    option (*fletch32str)(const char *str);
} std_crypto_hash;
std_crypto_hash __std_crypto_hash_struct;

void __hash_setup(){
    __std_crypto_hash_struct._minfo = mModuleNew("std.crypto.hash");
    __std_crypto_hash_struct.fletch32 = __crypto_hash_fletcher32;
    __std_crypto_hash_struct.fletch32str = __crypto_hash_fletcher32_string;
}

void __crypto_setup(){
    __hash_setup();
}

#define CRYPTO_HASH_INJECT_MOD {__std_crypto_hash_struct._minfo, &__std_crypto_hash_struct}
#endif
#define CRYPTO_MAIN_HEADER
