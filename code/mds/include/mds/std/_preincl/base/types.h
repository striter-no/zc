#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef enum {
    OPT_ERROR_TYPE,
    OPT_VARIABLE_TYPE
} __opt_tag;

typedef struct {} traceback;

typedef struct {
    const char *message;
    const char *type;
    int         code;
} error;

struct variable;
struct option;
typedef struct variable {
    void  *data;
    ssize_t size; // vb->size = num of elements
    bool alloced;

    void (*clone)(struct variable *self, struct variable vb);
    struct option (*lforeach) (struct variable *self, size_t elsize, struct option (^lmbd)(void* arg, size_t inx));
    struct option (*foreach)  (struct variable *self, size_t elsize, struct option (*lmbd)(void* arg, size_t inx));
    struct option (*dforeach) (struct variable *self, struct option (*lmbd)(void* arg, ssize_t inx));
    struct option (*ldforeach)(struct variable *self, struct option (^lmbd)(void* arg, ssize_t inx));
} variable;

struct option;
typedef struct option {
    __opt_tag tag;
    union {
        error    err;
        variable vrb;
    } variant;

    struct option (*catch) (struct option *self, int (^lmbd)(struct option *err));
    struct option (*fcatch)(struct option *self, int (*lmbd)(struct option *err));
} option;

// METHODS ======
option __opt_catch       (option *self, int (^lmbd)(option *err)); // done
option __opt_fcatch      (option *self, int (*lmbd)(option *err)); // done

void   __vrb_clone    (struct variable *self, struct variable vb);
option __vrb_lforeach (variable *vb, size_t elsize, option (^lmbd)(void* arg, size_t inx));
option __vrb_ldforeach(variable *vb, option (^lmbd)(void* arg, ssize_t inx));
option __vrb_foreach  (variable *vb, size_t elsize, option (*lmbd)(void* arg, size_t inx));
option __vrb_dforeach (variable *vb, option (*lmbd)(void* arg, ssize_t inx));

// FUNCTIONS =====
variable mvar(void *data, ssize_t size, bool alloced);
variable nv  (ssize_t value);
variable np  (void *ptr);
variable tv  (option opt);

bool   vis_equal(variable v1, variable v2);
option sumvar   (variable first, variable second);
option addvar   (variable *to, variable from);

variable shcopyvar(variable copy);
option   copyvar  (variable copy);
option   movevar  (variable *from, variable *to);
void     delvar   (variable *vr);
void    *discard  (option opt); // done

option opt_var(variable _var); // done
option opt    (void *data, ssize_t size, bool alloced); // done
void  *td     (option opt); // done
void  *tdf    (option opt, void (^defer)()); // done

bool  is_error(option opt); // done
error gerror  (option opt); // done
