#pragma once
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>
#include <mds/std/memory/allocators/generalPurpose.h>

static GeneralPurposeAllocator *gpa = NULL;

option talloc  (size_t size)              { return __alc_gpa_allocate(gpa, size); }
option tfree   (void *ptr)                { return __alc_gpa_free(gpa, ptr); }
option tcalloc (size_t num, size_t elsize){ return __alc_gpa_zeroalloc(gpa, num, elsize); }
option trealloc(void *ptr, size_t newsize){ return __alc_gpa_reallocate(gpa, ptr, newsize); }

void *galloc  (size_t size)              { return _catch(talloc(size), nv(0)).data; }
void  gfree   (void *ptr)                { tfree(ptr); }
void *gcalloc (size_t num, size_t elsize){ return _catch(tcalloc(num, elsize), nv(0)).data; }
void *grealloc(void *ptr, size_t newsize){ return _catch(trealloc(ptr, newsize), nv(0)).data; }

option __galc_init(){
    gpa = try(__alc_gpa_init()).data;
    return noerropt;
}

void __galc_end(){
    discard(__alc_gpa_end(gpa));
}