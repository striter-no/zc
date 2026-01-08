#ifndef NICE_OBJ_HEADER
#include <string.h>


struct object {
    void *(^create)(void*);
    void *(^destroy)(void*);

    void *(^copy)(void*);
    void *(^shallow_copy)(void*);
    void *(^move)(void*);
};

// void createo(){}
// void destroyo(){}

// void copyo(){}
// void shallow_copyo(){}
// void moveo(){}

#endif
#define NICE_OBJ_HEADER
