#ifndef STRINGS_HEADER
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/containers/array.h>
#include <mds/std/memory/sentinel.h>
#include "ctype.h"

typedef struct {
    variable data;
    ssize_t  len;
} String;

String __std_str_fromc(char data[]);
option __std_str_copyfrom(String other);
String __std_str_shadow(String other);
option __std_str_fromarr(array arr);
String __std_str_fromvar(variable val);
option __std_str_selfsum(String *self, String other);
option __std_str_ressum(String self, String other);
// split ?
// join ?

bool __std_str_isequal(String first, String second);
bool __std_str_isequalc(String first, const char* second);
String __std_str_slice(String self, size_t from, size_t num);
ssize_t __std_str_index(String self, String target);
ssize_t __std_str_indexc(String self, const char *target);
size_t __std_str_count(String self, String target);
size_t __std_str_countc(String self, const char* target);
bool __std_str_startswith(String self, String target);
bool __std_str_startswithc(String self, const char *target);
bool __std_str_endswith(String self, String target);
bool __std_str_endswithc(String self, const char *target);
bool __std_str_isint(String self);
bool __std_str_isfloat(String self);
bool __std_str_isnum(String self);
bool __std_str_isascii(String self);
option __std_str_replace(String self, String repl, String to_what);
option __std_str_replacec(String self, const char *repl, String to_what);
option __std_str_replacefc(String self, const char *repl, const char *to_what);
option __std_str_toint(String self);
option __std_str_tofloat(String self);
#ifdef STRINGS_IMPLEMENTATION

bool __std_str_isequalc(String first, const char* second){
    return first.len == strlen(second) && memcmp(first.data.data, second, sizeof(char) * first.len);
}

option __std_str_tofloat(String self){
    notimplemented;
}

String __std_str_shadow(String other){
    return (String){
        shcopyvar(other.data),
        -1
    };
}

option __std_str_ressum(String self, String other){
    var nd = try(sumvar(self.data, other.data));
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    String *out = try(absa->alloc(absa->real, sizeof(String))).data;
    if (!out) throw(
        "Std.Str: failed to resultative_summ, malloc() failed",
        "Std.Str.ResSum.Malloc.Failed",
        -1
    );
    out->data = nd;
    out->len = nd.size - 1;
    return opt(out, sizeof(String), true);
}

bool __std_str_endswithc(String self, const char *target){
    if (!self.data.data || !target || strlen(target) == 0 || self.len == 0) return false;
    if (self.len < strlen(target)) return false;
    
    return __std_str_isequalc(__std_str_slice(self, self.len - strlen(target), strlen(target)), target);
}

bool __std_str_endswith(String self, String target){
    if (!self.data.data || !target.data.data || target.len == 0 || self.len == 0) return false;
    if (self.len < target.len) return false;
    
    return __std_str_isequal(__std_str_slice(self, self.len - target.len, target.len), target);
}

String __std_str_fromvar(variable val){
    return (String){val, val.size};
}

option __std_str_replace(String self, String repl, String to_what){
    notimplemented;
    // if (!self.data.data || !repl.data.data || !to_what.data.data) throw(
    //     "Std.Str: replace failed self_str, repl_str or to_what_str *.data is NULL",
    //     "Std.Str.Replace.Args.IsNULL",
    //     1
    // );
}

option __std_str_copyfrom(String other){
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    String *out = try(absa->alloc(absa->real, sizeof(String))).data;
    if (!out) throw(
        "Std.Str: failed to get copy of the string, malloc() failed",
        "Std.Str.CopyFrom.Malloc.Failed",
        -1
    );

    out->data = try(copyvar(other.data));
    out->len = other.len;
    return opt(out, sizeof(String), true);
}

String __std_str_slice(String self, size_t from, size_t num){
    return (String){
        mvar(self.data.data + from, num - from, false),
        num - from
    };
}

size_t __std_str_countc(String self, const char* target){
    if (!self.data.data || !target || strlen(target) == 0 || self.len == 0) return 0;
    if (self.len < strlen(target)) return 0;
    if (__std_str_isequalc(self, target)) return 1;
    size_t o = 0;
    for (size_t i = 0; i < self.len - strlen(target); i++){
        if (__std_str_isequalc(
            __std_str_slice(self, i, self.len), 
            target
        )){
            o++;
        }
    }
    return o;
}

bool __std_str_isascii(String self){
    if (!self.data.data || self.len == 0) return false;
    char* str = (char*)self.data.data;
    
    for (int i = 0; i < self.len; i++) {
        if ((unsigned char)str[i] > 127) {
            return false;
        }
    }
    
    return true;
}

option __std_str_replacefc(String self, const char *repl, const char *to_what){
    notimplemented;
}

ssize_t __std_str_index(String self, String target){
    if (!self.data.data || !target.data.data || target.len == 0 || self.len == 0) return -1;
    if (self.len < target.len) return -1;
    if (__std_str_isequal(self, target)) return 0;
    for (size_t i = 0; i < self.len - target.len; i++){
        if (__std_str_isequal(
            __std_str_slice(self, i, self.len), 
            target
        )){
            return i;
        }
    }

    return -1;
}

bool __std_str_startswithc(String self, const char *target){
    if (!self.data.data || !target || strlen(target) == 0 || self.len == 0) return false;
    if (self.len < strlen(target)) return false;
    
    return __std_str_isequalc(__std_str_slice(self, 0, strlen(target)), target);
}

size_t __std_str_count(String self, String target){
    if (!self.data.data || !target.data.data || target.len == 0 || self.len == 0) return 0;
    if (self.len < target.len) return 0;
    if (__std_str_isequal(self, target)) return 1;
    size_t o = 0;
    for (size_t i = 0; i < self.len - target.len; i++){
        if (__std_str_isequal(
            __std_str_slice(self, i, self.len),
            target
        )){
            o++;
        }
    }
    return o;
}

option __std_str_selfsum(String *self, String other){
    if (!self) throw(
        "Std.Str: failed to selfsum, self ptr is NULL",
        "Std.Str.SelfSum.Ptr.IsNULL",
        1
    );
    if (other.len == 0) return noerropt;

    try(addvar(&self->data, other.data));
    self->len += other.len;
    return noerropt;
}

option __std_str_fromarr(array arr){
    String *out = try(arr.absa->alloc(arr.absa->real, sizeof(String))).data;
    if (!out) throw(
        "Std.Str: failed to get string from array, malloc() failed",
        "Std.Str.FromArray.Malloc.Failed",
        -1
    );

    if (arr.len == 0) {
        *out = (String){.data = mvar(NULL, 0, false), 0};
        return opt(out, sizeof(String), true);
    }

    out->data = mvar(
        try(arr.absa->alloc(arr.absa->real, sizeof(char) * arr.len)).data,
        sizeof(char) * arr.len,
        true
    );
    out->len = arr.len - 1;
    if (!out->data.data){
        try(arr.absa->free(arr.absa->real, out));
        throw(
        "Std.Str: failed to get string from array, malloc(2) failed",
        "Std.Str.FromArray.Malloc.2.Failed",
        1
        );
    }

    var sentinel = nv(0);
    if (!__std_mem_isDataSentineled(
        arr.elements, sizeof(variable) * arr.len, sizeof(variable), 
        &sentinel, sizeof(variable)
    )){
        throw(
            "Std.Str: failed to get string from array, array is not sentineled with \\0",
            "Std.Str.FromArray.InvalidFormat",
            1
        );
    }

    for (size_t i = 0; i < arr.len; i++){
        if (arr.elements[i].size != sizeof(char)) {
            delvar(&out->data);
            free(out);
            throw(
             "Std.Str: failed to get string from array, in array element(s) are not one-byte sized",
            "Std.Str.FromArray.InvalidFormat",
            2
            );
        }
        if (!arr.elements[i].data) throw(
         "Std.Str: failed to get string from array, in array element(s) are NULL",
        "Std.Str.FromArray.DataCorrupted",
        3
        );

        ((char*)out->data.data)[i] = ((char*)arr.elements[i].data)[0];
    }

    return opt(out, sizeof(String), true);
}

String __std_str_fromc(char data[]){
    return (String){
        mvar(data, strlen(data) + 1, false),
        strlen(data)
    };
}

bool __std_str_isfloat(String self){
    if (!self.data.data || self.len == 0) return false;
    
    char* str = (char*)self.data.data;
    int start = 0;
    bool dot_seen = false;
    
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (self.len == 1) return false;
    }
    
    for (int i = start; i < self.len; i++) {
        if (str[i] == '.') {
            if (dot_seen) return false;
            dot_seen = true;
            if (i == start) return false;
            if (i == self.len - 1) return false;
        } 
        else if (!isdigit((unsigned char)str[i])) {
            return false;
        }
    }
    
    return dot_seen;
}

option __std_str_replacec(String self, const char *repl, String to_what){
    notimplemented;
}

bool __std_str_isequal(String first, String second){
    return first.len == second.len && vis_equal(first.data, second.data);
}

bool __std_str_isint(String self){
    if (!self.data.data || self.len == 0) return false;
    char* str = (char*)self.data.data;
    int start = 0;
    
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (self.len == 1) return false;
    }
    
    for (int i = start; i < self.len; i++)
        if (!isdigit((unsigned char)str[i]))
            return false;

    return true;
}

bool __std_str_isnum(String self){
    if (!self.data.data || self.len == 0) return false;
    return __std_str_isint(self) || __std_str_isfloat(self);
}

option __std_str_toint(String self){
    notimplemented;
}

ssize_t __std_str_indexc(String self, const char *target){
    if (!self.data.data || !target || strlen(target) == 0 || self.len == 0) return -1;
    if (self.len < strlen(target)) return -1;
    if (__std_str_isequalc(self, target)) return 0;
    for (size_t i = 0; i < self.len - strlen(target); i++){
        if (__std_str_isequalc(
            __std_str_slice(self, i, self.len), 
            target
        )){
            return i;
        }
    }

    return -1;
}

bool __std_str_startswith(String self, String target){
    if (!self.data.data || !target.data.data || target.len == 0 || self.len == 0) return false;
    if (self.len < target.len) return false;
    
    return __std_str_isequal(__std_str_slice(self, 0, target.len), target);
}


#endif
#endif
#define STRINGS_HEADER
