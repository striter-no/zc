#ifndef METHODS_HEADER
#include "preproc.h"
#include <Block.h>

#define __struct_self(_type) \
    _type* self = malloc(sizeof(_type));\
    if (!self) throw(TOSTRING(_type) " creation failed, malloc() failed", TOSTRING(_type) ".Creation.Malloc.Failed", -1);
#define __struct_initializer(_type, block, ...) option _type##CreateStruct(__VA_ARGS__) {__struct_self(_type); block; return opt(self, sizeof(_type), true);}
#define __struct_method(name, mainfunc, ...) __dwl({self->name = Block_copy(^(__VA_ARGS__){mainfunc(self);});});

// __struct_initializer(MyStruct, {
//     self->data = data;
//     // methods
//     __struct_method(print, __method_print, void);
// }, int data);

#ifdef METHODS_IMPLEMENTATION


#endif
#endif
#define METHODS_HEADER
