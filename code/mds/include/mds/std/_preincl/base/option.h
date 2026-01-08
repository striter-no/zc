#ifndef OPTION_HEADER
#include "preproc.h"

option __opt_catch(option *self, int (^lmbd)(option *err));
option __opt_fcatch(option *self, int (*lmbd)(option *err));
option opt_var(variable _var);
option opt(void *data, ssize_t size, bool alloced);
void *discard(option opt);
void *td(option opt);
void *tdf(option opt, void (^_defer)());

bool is_error(option opt);
error gerror (option opt);

#ifdef OPTION_IMPLEMENTATION

option opt_var(variable _var){
    return (option){
        .tag = OPT_VARIABLE_TYPE,
        .variant = {
            .vrb = {
                .alloced = _var.alloced,
                .data = _var.data,
                .size = _var.size,
                .foreach = __vrb_foreach,
                .lforeach = __vrb_lforeach,
                .dforeach = __vrb_dforeach,
                .ldforeach = __vrb_ldforeach
            }
        }
    };
}

error gerror (option opt){
    if (opt.tag != OPT_ERROR_TYPE) panic("gerror: opt is not error");
    return opt.variant.err;
}

void *tdf(option opt, void (^_defer)()){
    if (opt.tag == OPT_ERROR_TYPE){
        _defer();
        return NULL;
    }

    return tv(opt).data;
}

option opt(void *data, ssize_t size, bool alloced){
    return (option){
        .tag = OPT_VARIABLE_TYPE,
        .variant = {.vrb = mvar(data, size, alloced)},
        .catch = __opt_catch,
        .fcatch = __opt_fcatch
    };
}

option __opt_catch(option *self, int (^lmbd)(option *err)){
    if (!self) panic("option::catch() option* is NULL");
    if (self->tag == OPT_ERROR_TYPE && lmbd(self)){
        self->tag = OPT_VARIABLE_TYPE;
        return *self;
    } else if(self->tag == OPT_ERROR_TYPE){
        return *self;
    } 
    return *self;
}

bool is_error(option opt){
    return opt.tag == OPT_ERROR_TYPE;
}

void *discard(option opt){
    return opt.tag == OPT_ERROR_TYPE ? NULL: tv(opt).data;
}

option __opt_fcatch(option *self, int (*lmbd)(option *err)){
    if (!self) panic("option::catch() option* is NULL");
    if (self->tag == OPT_ERROR_TYPE && lmbd(self)){
        self->tag = OPT_VARIABLE_TYPE;
        return *self;
    } else if(self->tag == OPT_ERROR_TYPE){
        return *self;
    } 
    return *self;
}

void *td(option opt){
    return tv(opt).data;
}


#endif
#endif
#define OPTION_HEADER
