#pragma once
#include <mds/std/_preincl/base.h>
#include <stdbool.h>
#include "array.h"
#include "table.h"
#include <mds/std/memory/allocators/abstract.h>

#ifndef INIT_HSHT_CAPACITY
#define INIT_HSHT_CAPACITY 50
#endif

size_t __hash_ptr(void *ptr, size_t table_size) ;
size_t __hash_var(variable vr, size_t table_size) ;
typedef struct {
    array table;

    size_t len;
    size_t capacity;
    AbstractAllocator *absa;
} hashtable;

option __hsht_new();
option __hsht_rehash(hashtable *ht);
option __hsht_set(hashtable *ht, variable key, variable val);
option __hsht_get(hashtable *ht, variable key);
option __hsht_delete(hashtable *ht, variable key) ;
void __hsht_dfclean(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr));
option __hsht_free(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr));
#ifdef HASHTABLE_IMPLEMENTATION

size_t __hash_var(variable vr, size_t table_size) {
    if (!vr.data) return 0;
    if (vr.size == 0) return __hash_ptr(vr.data, table_size);
    
    uint64_t hash = 14695981039346656037UL;
    unsigned char* data = (unsigned char*)vr.data;
    size_t limit = vr.size > 1024 ? 1024 : vr.size;
    
    for (size_t i = 0; i < limit; i++) {
        hash ^= data[i];
        hash *= 1099511628211UL;
    }

    hash ^= vr.size;
    hash *= 1099511628211UL;
    
    return hash % table_size;
}

option __hsht_delete(hashtable *ht, variable key) {
    if (!ht) throw("Failed to delete from hashtable, hashtable ptr is NULL", "HSHT.Delete.Ptr.IsNULL", -1);

    size_t index = __hash_var(key, ht->capacity);
    array* bucket = (array*)(ht->table.elements[index].data);
    
    // fprintf(stderr, "[hsht] trying to delete by %p (->inx: %zu, bucklen: %zu)\n", key.data, index, bucket->len);
    for (size_t i = 0; i < bucket->len; i++) {
        pair* pr = (pair*)(bucket->elements[i].data);
        // fprintf(stderr, "-> hsht: in for: %zu/%zu: cmp %p (%p)<->%p\n", i + 1, bucket->len, pr->first.data, pr->second.data, key.data);
        if (vis_equal(pr->first, key)) {
            // fprintf(stderr, ":: hsht: found %p = %p\n", pr->first.data, key.data);
            delvar(&pr->first);
            delvar(&pr->second);
            try(ht->absa->free(ht->absa->real, pr));
            
            if (bucket->len > 1 && i < bucket->len - 1) {
                bucket->elements[i] = bucket->elements[bucket->len - 1];
            }
            bucket->len--;
            ht->len--;
            
            return noerropt;
        }
    }
    
    throw("Key not found in hashtable", "HSHT.Delete.Key.NotFound", 1);
}

option __hsht_get(hashtable *ht, variable key){
    if (!ht) throw("Failed to get value from hashtable, hashtable ptr is NULL", "HSHT.Get.Ptr.IsNULL", -1);
    
    size_t inx = __hash_var(key, ht->capacity);

    var bucket = (array*)(ht->table.elements[inx].data);
        
    for (size_t k = 0; k < bucket->len; k++){
        var pr = (pair*)(bucket->elements[k].data);
        if (vis_equal(pr->first, key)){
            return opt_var(pr->second);
        }
    }

    throw("HSHT failed to get value by key, not found", "HSHT.Get.Key.NotFound", 1);
}

void __hsht_dfclean(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr)){
    if (!ht) return;

    var hshdefer = lambda(variable *vr){
        pair *p = vr->data;
        if (!p) return;

        if (defer_key) defer_key(&p->first);
        if (defer_val) defer_val(&p->second);
        discard(ht->absa->free(ht->absa->real, p));
    };

    for (size_t i = 0; i < ht->capacity; i++){
        __array_dfclean(ht->table.elements[i].data, hshdefer);
    }
}

size_t __hash_ptr(void *ptr, size_t table_size) {
    return ((uintptr_t)ptr * 2654435761UL) % table_size;
}

option __hsht_free(hashtable *ht, void (^defer_key)(variable *vr), void (^defer_val)(variable *vr)){
    __hsht_dfclean(ht, defer_key, defer_val);

    for (size_t i = 0; i < ht->capacity; i++){
        try(__array_free(ht->table.elements[i].data));
        try(ht->absa->free(ht->absa->real, ht->table.elements[i].data));
    }
    try(__array_free(&ht->table));
    ht->len = 0;
    ht->capacity = 0;
    return noerropt;
}

option __hsht_new(){
    // fprintf(stderr, "__hsht_new: called\n");
    AbstractAllocator* absa = try(global.get(".absa.raw")).data;
    hashtable *ht = try(absa->alloc(absa->real, sizeof(hashtable))).data;
    // fprintf(stderr, "__hsht_new: absa ok\n");

    *ht = (hashtable){
        .table = __array_new_ca(absa),
        .len = 0,
        .capacity = INIT_HSHT_CAPACITY,
        .absa = absa
    };

    // fprintf(stderr, "__hsht_new_init done\n");

    tryd(__array_reserve(&ht->table, ht->capacity), lambda(){
        td(ht->absa->free(ht->absa->real, ht));
    });
    for (size_t i = 0; i < ht->capacity; i++) {
        array* bucket = try(ht->absa->alloc(ht->absa->real, sizeof(array))).data;
        if (!bucket) {
            for (size_t j = 0; j < i; j++) {
                try(ht->absa->free(ht->absa->real, ((array*)ht->table.elements[j].data)));
            }
            try(__array_free(&ht->table));
            try(ht->absa->free(ht->absa->real, ht));
            throw("Failed to allocate bucket", "HSHT.New.Bucket.Malloc.Failed", -2);
        }
        *bucket = __array_new_ca(absa);
        ht->table.elements[i] = mvar(bucket, sizeof(array), true);
    }

    return opt(ht, sizeof(hashtable), true);
}

option __hsht_rehash(hashtable *ht){
    if (!ht) throw("Failed to rehash hashtable, hashtable ptr is NULL", "HSHT.Rehash.Ptr.IsNULL", -1);
    // fprintf(stderr, "[hsht] rehashing\n");

    size_t oldcap = ht->capacity;
    size_t newcap = oldcap * 2;
    
    array new_table = __array_new_ca(ht->absa);
    try(__array_reserve(&new_table, newcap));
    
    for (size_t i = 0; i < newcap; i++) {
        array* new_bucket = try(ht->absa->alloc(ht->absa->real, sizeof(array))).data;
        *new_bucket = __array_new_ca(ht->absa);
        new_table.elements[i] = mvar(new_bucket, sizeof(array), true);
    }
    
    for (size_t i = 0; i < oldcap; i++) {
        array* old_bucket = (array*)(ht->table.elements[i].data);
        
        for (size_t k = 0; k < old_bucket->len; k++) {
            pair* pr = (pair*)(old_bucket->elements[k].data);
            
            size_t new_index = __hash_ptr(pr->first.data, newcap);
            array* new_bucket = (array*)(new_table.elements[new_index].data);
            
            try(__array_shpushback(new_bucket, mvar(pr, sizeof(pair), true)));
            old_bucket->elements[k].data = NULL;
        }
        
        try(__array_free(old_bucket));
        try(ht->absa->free(ht->absa->real, old_bucket));
        ht->table.elements[i].data = NULL;
    }
    
    try(__array_free(&ht->table));
    ht->table = new_table;
    ht->capacity = newcap;
    
    return noerropt;
}

option __hsht_set(hashtable *ht, variable key, variable val){
    if (!ht) throw("Failed to insert new pair in hashtable, hashtable ptr is NULL", "HSHT.Insert.Ptr.IsNULL", -1);

    
    size_t index = __hash_var(key, ht->capacity);
    // fprintf(stderr, "[hsht][set] by key: %p, val: %p (->inx: %zu)\n", key.data, val.data, index);
    var bucket = (array*)(ht->table.elements[index].data);
    
    bool found = false;
    for (size_t i = 0; i < bucket->len; i++){
        var pr = (pair*)(bucket->elements[i].data);
        if (vis_equal(pr->first, key)){
            pr->second = shcopyvar(val); // does not need a defer

            found = true;
            break;
        }
    }

    if (!found){
        pair *pr = try(ht->absa->alloc(ht->absa->real, sizeof(pair))).data;
        if (!pr) throw("Failed to insert new pair in hashtable, malloc failed", "HSHT.Insert.Malloc.Failed", -2);
        pr->first = shcopyvar(key); // fixed
        pr->second = shcopyvar(val); // fixed
        
        tryd(__array_shpushback(bucket, mvar(pr, sizeof(pair), true)), lambda(){
            ht->absa->free(ht->absa->real, pr);
        });
        ht->len++;
    }

    if (ht->len > ht->capacity * 0.7){
        try(__hsht_rehash(ht)); // does not need a defer
    }

    return noerropt;
}


#endif