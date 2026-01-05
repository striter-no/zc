#pragma once
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
