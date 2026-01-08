#ifndef STRING_MAIN_HEADER
#include <mds/genmod.h>
#include "mds/std/string/fmt.h"
#include "./strings.h"

typedef struct {
    Module _minfo;

    bool (*isequal)(String first, String second);
    bool (*isequalc)(String first, const char* second);
    bool (*startswith)(String self, String target);
    bool (*startswithc)(String self, const char *target);
    bool (*endswith)(String self, String target);
    bool (*endswithc)(String self, const char *target);
    bool (*isint)(String self);
    bool (*isfloat)(String self);
    bool (*isnum)(String self);
    bool (*isascii)(String self);

    ssize_t (*index)(String self, String target);
    ssize_t (*indexc)(String self, const char *target);
    size_t  (*count)(String self, String target);
    size_t  (*countc)(String self, const char* target);

    String (*fromc)(char data[]);
    String (*shadow)(String other);
    String (*fromvar)(variable val);
    String (*slice)(String self, size_t from, size_t num);

    option (*copyfrom)(String other);
    option (*fromarr)(array arr);
    option (*selfsum)(String *self, String other);
    option (*ressum)(String self, String other);

    option (*replace)(String self, String repl, String to_what);
    option (*replacec)(String self, const char *repl, String to_what);
    option (*replacefc)(String self, const char *repl, const char *to_what);
    option (*toint)(String self);
    option (*tofloat)(String self);
} std_str;
std_str __std_str;

typedef struct {
    Module _minfo;

    option (*format)(const char *fmt, ...);
} std_fmt;
std_fmt __fmt;

void __str_setup(){
    __std_str._minfo = mModuleNew("std.str");
    __std_str.isequal = __std_str_isequal;
    __std_str.isequalc = __std_str_isequalc;
    __std_str.startswith = __std_str_startswith;
    __std_str.startswithc = __std_str_startswithc;
    __std_str.endswith = __std_str_endswith;
    __std_str.endswithc = __std_str_endswithc;
    __std_str.isint = __std_str_isint;
    __std_str.isfloat = __std_str_isfloat;
    __std_str.isnum = __std_str_isnum;
    __std_str.isascii = __std_str_isascii;
    __std_str.index = __std_str_index;
    __std_str.indexc = __std_str_indexc;
    __std_str.count = __std_str_count;
    __std_str.countc = __std_str_countc;
    __std_str.fromc = __std_str_fromc;
    __std_str.shadow = __std_str_shadow;
    __std_str.fromvar = __std_str_fromvar;
    __std_str.slice = __std_str_slice;
    __std_str.copyfrom = __std_str_copyfrom;
    __std_str.fromarr = __std_str_fromarr;
    __std_str.selfsum = __std_str_selfsum;
    __std_str.ressum = __std_str_ressum;
    __std_str.replace = __std_str_replace;
    __std_str.replacec = __std_str_replacec;
    __std_str.replacefc = __std_str_replacefc;
    __std_str.toint = __std_str_toint;
    __std_str.tofloat = __std_str_tofloat;
}

void __fmt_setup(){
    __fmt._minfo = mModuleNew("std.fmt");
    __fmt.format = format;
}

void __strings_setup(){
    __fmt_setup();
    __str_setup();
}

#define FMT_INJECT_MOD {__fmt._minfo, &__fmt}
#define STR_INJECT_MOD {__std_str._minfo, &__std_str}
#endif
#define STRING_MAIN_HEADER
