#pragma once
#include "preproc.h"

void __vrb_clone(struct variable *self, struct variable vb);
option __vrb_lforeach(variable *vb, size_t elsize, option (^lmbd)(void* arg, size_t inx));
option __vrb_ldforeach(variable *vb, option (^lmbd)(void* arg, ssize_t inx));
option __vrb_foreach(variable *vb, size_t elsize, option (*lmbd)(void* arg, size_t inx));
option __vrb_dforeach(variable *vb, option (*lmbd)(void* arg, ssize_t inx));
variable mvar(void *data, ssize_t size, bool alloced);
variable nv(ssize_t value);
variable np(void *ptr);
variable tv(option opt);
bool vis_equal(variable v1, variable v2);
option sumvar(variable first, variable second);
option addvar(variable *to, variable from);
variable shcopyvar(variable copy);
option copyvar(variable copy);
option movevar(variable *from, variable *to);
void delvar(variable *vr);

#ifdef VARIABLE_IMPLEMENTATION

option copyvar(variable copy){
    
    variable nvar = mvar(NULL, copy.size, true);
    if (copy.data != NULL){
        nvar.data = malloc(nvar.size);
        
        if (!nvar.data) throw(
            "Failed at std.copyvar, malloc failed",
            "Std.Copyvar.Malloc.Failed",
            -1
        );
    } else
        nvar.data = NULL;

    if (copy.data) 
        memcpy(nvar.data, copy.data, copy.size);

    return opt_var(nvar);
}

option __vrb_lforeach(variable *vb, size_t elsize, option (^lmbd)(void* arg, size_t inx)){
    for (size_t i = 0; i < vb->size; i++){
        try(lmbd(vb->data + i * elsize, i));
    }
    return noerropt;
}

option sumvar(variable first, variable second){
    variable new_var = mvar(NULL, first.size + second.size, true);
    new_var.data = malloc(new_var.size);
    if (!new_var.data) throw(
        "Failed at std.sumvar, malloc failed",
        "Std.Sumvar.Malloc.Failed",
        -1
    );
    
    if (!first.data && !second.data) throw(
        "Failed at std.sumvar, both parts are NULL",
        "Std.Sumvar.IncorrectParts",
        1
    );

    if (first.data)  memcpy(new_var.data, first.data, first.size);
    if (second.data) memcpy(new_var.data + first.size, second.data, second.size);

    return opt_var(new_var);
}

option addvar(variable *to, variable from){
    variable ntemp = try(sumvar(*to, from));
    delvar(to);
    movevar(&ntemp, to);
    
    return noerropt;
}

variable np(void *ptr){
    return mvar(ptr, sizeof(void*), false);
}

variable nv(ssize_t value){
    return mvar(NULL, value, false);
}

option movevar(variable *from, variable *to){
    delvar(to);
    *to = try(copyvar(*from));
    delvar(from);

    return noerropt;
}

option shallowmove(variable *from, variable *to){
    delvar(to);
    *to = *from;
    delvar(from);

    return noerropt;
}

variable tv(option opt){
    if (opt.tag == OPT_ERROR_TYPE){
        fprintf(stderr, "> prepanic: %s:%s\n", opt.variant.err.type, opt.variant.err.message);
        panic("getv on error type");
    }

    return opt.variant.vrb;
}

variable shcopyvar(variable copy){
    return (variable){
        .data = copy.data,
        .size = copy.size
    };
}

void __vrb_clone(struct variable *self, struct variable vb){
    self->data = vb.data;
    self->size = vb.size;
}

option __vrb_ldforeach(variable *vb, option (^lmbd)(void* arg, ssize_t inx)){
    size_t offset = try(lmbd(NULL, -1)).size;
    for (size_t i = 0; i < vb->size; i++){
        offset += try(lmbd(vb->data + offset, i)).size;
    }
    return noerropt;
}

option __vrb_dforeach(variable *vb, option (*lmbd)(void* arg, ssize_t inx)){
    size_t offset = try(lmbd(NULL, -1)).size;
    for (size_t i = 0; i < vb->size; i++){
        offset += try(lmbd(vb->data + offset, i)).size;
    }
    return noerropt;
}

void delvar(variable *vr){
    if (!vr) return;
    if (!vr->data) return;

    if (vr->alloced) free(vr->data);
    vr->data = NULL;
    vr->size = 0;
}

variable mvar(void *data, ssize_t size, bool alloced){
    return (variable){
        .alloced = alloced,
        .data = data,
        .size = size,
        .foreach = __vrb_foreach,
        .lforeach = __vrb_lforeach,
        .dforeach = __vrb_dforeach,
        .ldforeach = __vrb_ldforeach,
        .clone = __vrb_clone
    };
}

option __vrb_foreach(variable *vb, size_t elsize, option (*lmbd)(void* arg, size_t inx)){
    for (size_t i = 0; i < vb->size; i++){
        try(lmbd(vb->data + i * elsize, i));
    }
    return noerropt;
}

bool vis_equal(variable v1, variable v2){
    if (v1.data == NULL && v2.data == NULL) return true;
    if (v1.data == NULL || v2.data == NULL) return false;
    if (v1.size != v2.size)                 return false;
    if (v1.size == 0 && v2.size == 0)       return v1.data == v2.data;
    
    return memcmp(v1.data, v2.data, v1.size) == 0;
}


#endif