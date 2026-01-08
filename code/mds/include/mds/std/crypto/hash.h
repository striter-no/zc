#ifndef HASH_HEADER
#include "mds/std/_preincl/base.h"

option __crypto_hash_fletcher32(const u16* data, size_t len) ;
option __crypto_hash_fletcher32_string(const char* str) ;

#ifdef HASH_IMPLEMENTATION

option __crypto_hash_fletcher32_string(const char* str) {
    if (!str) throw(
        "Crypto.Hash fletch32_string failed, str ptr is NULL",
        "Crypto.Hash.Fletcher32_String.Failed",
        -1
    );
    
    size_t len = strlen(str);
    size_t padded_len = (len + 1) / 2;
    u16* data = (u16*)str;
    return __crypto_hash_fletcher32(data, padded_len);
}

option __crypto_hash_fletcher32(const u16* data, size_t len) {
    if (!data) throw(
        "Crypto.Hash fletch32 failed, data ptr is NULL",
        "Crypto.Hash.Fletcher32.Failed",
        -1
    );
    u32 sum1 = 0xffff, sum2 = 0xffff;

    while (len) {
        size_t tlen = len > 360 ? 360 : len;
        len -= tlen;

        do {
            sum1 += *data++;
            sum2 += sum1;
        } while (--tlen);

        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return opt(NULL, sum2 << 16 | sum1, false);
}


#endif
#endif
#define HASH_HEADER
