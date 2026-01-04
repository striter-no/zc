#pragma once
#include <mds/std/_preincl/base.h>
#include <mds/genmod.h>

#include "queue.h"
#include "array.h"
#include "table.h"
#include "hashtable.h"

typedef struct {
    Module _minfo;

    array  (*new)();
    option (*fromc)(const void *data, size_t elsize, size_t len);
    option (*reserve)(array *ptr, size_t len);
    option (*refat)(array *ptr, size_t index);
    option (*at)(array *ptr, size_t index);
    option (*delat)(array *ptr, size_t index);
    option (*index)(array *ptr, variable vr);
    option (*pushback)(array *ptr, variable vr);
    option (*shpushback)(array *ptr, variable vr);
    option (*pop)(array *ptr);
    option (*extendto)(array *ptr, size_t newlen);
    void   (*dfclean)(array *ptr, void (^defer)(variable *vr));
    option (*free)(array *ptr);

    void   (*foreach)(array *ptr, int (^expr)(variable *, size_t));
} std_array;
std_array __arrays_struct;

typedef struct {
    Module _minfo;

    kvtable (*new)();
    option  (*free)(kvtable *kt);
    option  (*set)(kvtable *kt, variable key, variable val);
    option  (*sets)(kvtable *kt, const char* key, variable val);
    option  (*shset)(kvtable *kt, variable key, variable val);
    option  (*shsets)(kvtable *kt, const char* key, variable val);
    option  (*delat)(kvtable *kt, variable key);
    option  (*at)(kvtable *kt, variable key);
    option  (*ats)(kvtable *kt, const char* key);
    option  (*refat)(kvtable *kt, variable key);
    option  (*key_by_val)(kvtable *kt, variable val);
    option  (*dfclean)(kvtable *kt, void (^keys_defer)(variable *),
                                    void (^vals_defer)(variable *));

} std_kvtable;
std_kvtable __kvtable_struct;

typedef struct {
    Module _minfo;
 
    queue  (*new)();
    option (*free)(queue* q);
    void   (*dfclean)(queue *q, void (^defer)(variable *vr));
    option (*push)(queue *q, variable vr);
    option (*pop)(queue *q);
    option (*top)(queue q);
} std_queue;
std_queue __queue_struct;

typedef struct {
    Module _minfo;

    option (*new)();
    option (*rehash)(hashtable *ht);
    option (*set)(hashtable *ht, variable key, variable val);
    option (*get)(hashtable *ht, variable key);
    option (*del)(hashtable *ht, variable key);
    void   (*dfclean)(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr));
    option (*free)(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr));
} std_htable;
std_htable __hashtable_struct;

void __hashtable_setup(){
    __hashtable_struct._minfo = mModuleNew("std.htable");
    __hashtable_struct.new = __hsht_new;
    __hashtable_struct.free = __hsht_free;
    __hashtable_struct.dfclean = __hsht_dfclean;
    __hashtable_struct.set = __hsht_set;
    __hashtable_struct.get = __hsht_get;
    __hashtable_struct.del = __hsht_delete;
    __hashtable_struct.rehash = __hsht_rehash;
}

void __queue_setup(){
    __queue_struct._minfo = mModuleNew("std.queue");
    __queue_struct.new = __queue_new;
    __queue_struct.free = __queue_free;
    __queue_struct.dfclean = __queue_dfclean;
    __queue_struct.push = __queue_push;
    __queue_struct.pop = __queue_pop;
    __queue_struct.top = __queue_top;
}

void __kvtables_setup(){
    __kvtable_struct._minfo = mModuleNew("std.kvtable");
    __kvtable_struct.new = __kvtable_new;
    __kvtable_struct.free = __kvtable_free;
    __kvtable_struct.set = __kvtable_set;
    __kvtable_struct.sets = __kvtable_sets;
    __kvtable_struct.shsets = __kvtable_shallow_sets;
    __kvtable_struct.shset = __kvtable_shallow_set;
    __kvtable_struct.delat = __kvtable_delat;
    __kvtable_struct.at = __kvtable_at;
    __kvtable_struct.ats = __kvtable_ats;
    __kvtable_struct.refat = __kvtable_refat;
    __kvtable_struct.key_by_val = __kvtable_key_by_val;
    __kvtable_struct.dfclean = __kvtable_dfclean;
}

void __arrays_setup(){
    __arrays_struct._minfo = mModuleNew("std.array");
    __arrays_struct.new = __array_new;
    __arrays_struct.fromc = __array_fromc;
    __arrays_struct.refat = __array_refat;
    __arrays_struct.at = __array_at;
    __arrays_struct.delat = __array_delat;
    __arrays_struct.index = __array_index;
    __arrays_struct.pushback = __array_pushback;
    __arrays_struct.pop = __array_pop;
    __arrays_struct.dfclean = __array_dfclean;
    __arrays_struct.free = __array_free;
    __arrays_struct.foreach = __array_foreach;
    __arrays_struct.reserve = __array_reserve;
    __arrays_struct.extendto = __array_extend_to;
    __arrays_struct.shpushback = __array_shpushback;
}

void __containers_setup(){
    __arrays_setup();
    __kvtables_setup();
    __queue_setup();
    __hashtable_setup();
}

#define ARR_INJECT_MOD {__arrays_struct._minfo, &__arrays_struct}
#define KVT_INJECT_MOD {__kvtable_struct._minfo, &__kvtable_struct}
#define QUEUE_INJECT_MOD {__queue_struct._minfo, &__queue_struct}
#define HSHT_INJECT_MOD {__hashtable_struct._minfo, &__hashtable_struct}
