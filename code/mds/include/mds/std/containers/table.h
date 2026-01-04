#pragma once
#include <mds/std/_preincl/base.h>
#include "array.h"

typedef struct {
    variable first;
    variable second;
} pair;

typedef struct {
    array   keys;
    array values;

    size_t len;
} kvtable;

kvtable __kvtable_new();
option __kvtable_free(kvtable *kt);
option __kvtable_set(kvtable *kt, variable key, variable val);
option __kvtable_shallow_set(kvtable *kt, variable key, variable val);
option __kvtable_sets(kvtable *kt, const char *_key, variable val);
option __kvtable_shallow_sets(kvtable *kt, const char *_key, variable val);
option __kvtable_delat(kvtable *kt, variable key);
option __kvtable_at(kvtable *kt, variable key);
option __kvtable_ats(kvtable *kt, const char *_key);

option __kvtable_refat(kvtable *kt, variable key);
option __kvtable_key_by_val(kvtable *kt, variable val);
option __kvtable_dfclean(kvtable *kt, void (^keys_defer)(variable *key), void (^vals_defer)(variable *val));
#ifdef TABLE_IMPLEMENTATION

option __kvtable_free(kvtable *kt){
    try(__array_free(&kt->keys));
    try(__array_free(&kt->values));
    kt->len = 0;
    return noerropt;
}

option __kvtable_shallow_set(kvtable *kt, variable key, variable val){
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size == -1){
        try(__array_shpushback(&kt->keys, key));
        try(__array_shpushback(&kt->values, val));
        kt->len++;
    } else {
        var valptr = ((variable*)try(__array_refat(&kt->values, inx.size)).data);
        var keyptr = ((variable*)try(__array_refat(&kt->keys, inx.size)).data);
        delvar(keyptr);
        delvar(valptr);
        
        *keyptr = shcopyvar(key);
        *valptr = shcopyvar(val);
    }
    return noerropt;
}

option __kvtable_at(kvtable *kt, variable key){
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size != -1){
        return opt_var(try(__array_at(&kt->values, inx.size)));
    }

    throw(
        "KeyValue Table, cannot delete element at key, no such key", 
        "KVTable.Delat.NoKey",
        1
    );
}

kvtable __kvtable_new(){
    return (kvtable){
        .keys = __array_new(),
        .values = __array_new(),
        .len = 0
    };
}

option __kvtable_sets(kvtable *kt, const char *_key, variable val){
    var key = mvar((void*)_key, strlen(_key) + 1, false);
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size == -1){
        try(__array_pushback(&kt->keys, key));
        try(__array_pushback(&kt->values, val));
        kt->len++;
    } else {
        var valptr = ((variable*)try(__array_refat(&kt->values, inx.size)).data);
        var keyptr = ((variable*)try(__array_refat(&kt->keys, inx.size)).data);
        delvar(keyptr);
        delvar(valptr);
        
        *keyptr = try(copyvar(key));
        *valptr = try(copyvar(val));
    }
    return noerropt;
}

option __kvtable_delat(kvtable *kt, variable key){
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size != -1){
        var valptr = ((variable*)try(__array_refat(&kt->values, inx.size)).data);
        var keyptr = ((variable*)try(__array_refat(&kt->keys, inx.size)).data);
        delvar(valptr);
        delvar(keyptr);

        kt->len--;
        return noerropt;
    }

    throw(
        "KeyValue Table, cannot delete element at key, no such key", 
        "KVTable.Delat.NoKey",
        1
    );
}

option __kvtable_key_by_val(kvtable *kt, variable val){
    for (size_t i = 0; i < kt->len; i++){
        if (kt->values.elements[i].size == val.size && (
            (val.data == NULL && val.size != 0) || memcmp(
                val.data, kt->values.elements[i].data, val.size
            ) == 0
        )){
            return opt_var(try(__array_at(&kt->keys, i)));
        }
    }
    
    throw(
        "KeyValue Table, failed to get key by value",
        "KVTable.KeyByVal.Failed",
        1
    );
}

option __kvtable_ats(kvtable *kt, const char *_key){
    variable key = mvar((void*)_key, strlen(_key) + 1, false);

    var inx = try(__array_index(&kt->keys, key));
    if (inx.size != -1){
        return opt_var(try(__array_at(&kt->values, inx.size)));
    }

    fprintf(stderr, "> prethrow: key: %s\n", _key);
    throw(
        "KeyValue Table, cannot get element at key, no such key",
        "KVTable.AtS.NoKey",
        1
    );
}

option __kvtable_set(kvtable *kt, variable key, variable val){
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size == -1){
        try(__array_pushback(&kt->keys, key));
        try(__array_pushback(&kt->values, val));
        kt->len++;
    } else {
        var valptr = ((variable*)try(__array_refat(&kt->values, inx.size)).data);
        var keyptr = ((variable*)try(__array_refat(&kt->keys, inx.size)).data);
        delvar(keyptr);
        delvar(valptr);
        
        *keyptr = try(copyvar(key));
        *valptr = try(copyvar(val));
    }
    return noerropt;
}

option __kvtable_refat(kvtable *kt, variable key){
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size != -1){
        return opt_var(try(__array_refat(&kt->values, inx.size)));
    }

    throw(
        "KeyValue Table, cannot delete element at key, no such key", 
        "KVTable.Delat.NoKey",
        1
    );
    
    return noerropt;
}

option __kvtable_shallow_sets(kvtable *kt, const char *_key, variable val){
    var key = mvar((void*)_key, strlen(_key) + 1, false);
    var inx = try(__array_index(&kt->keys, key));
    if (inx.size == -1){
        try(__array_shpushback(&kt->keys, key));
        try(__array_shpushback(&kt->values, val));
        kt->len++;
    } else {
        var valptr = ((variable*)try(__array_refat(&kt->values, inx.size)).data);
        var keyptr = ((variable*)try(__array_refat(&kt->keys, inx.size)).data);
        delvar(keyptr);
        delvar(valptr);
        
        *keyptr = shcopyvar(key);
        *valptr = shcopyvar(val);
    }
    return noerropt;
}

option __kvtable_dfclean(kvtable *kt, void (^keys_defer)(variable *key), void (^vals_defer)(variable *val)){
    __array_dfclean(&kt->keys, keys_defer);
    __array_dfclean(&kt->values, vals_defer);
    kt->len = 0;

    return noerropt;
}


#endif