#ifndef GALLC_HEADER
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

void gdelvar(variable *vr){
    if (!vr) return;
    if (!vr->data) return;

    if (vr->alloced) gfree(vr->data);
    vr->data = NULL;
    vr->size = 0;
}

option gcopyvar(variable copy){
    variable nvar = mvar(NULL, copy.size, true);
    if (copy.data != NULL){
        nvar.data = galloc(nvar.size);
        
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

option gmovevar(variable *from, variable *to){
    gdelvar(to);
    *to = try(gcopyvar(*from));
    gdelvar(from);

    return noerropt;
}

option gsumvar(variable first, variable second){
    variable new_var = mvar(NULL, first.size + second.size, true);
    new_var.data = galloc(new_var.size);
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
    if (second.data) memcpy(((u8*)new_var.data) + first.size, second.data, second.size);

    return opt_var(new_var);
}

option gaddvar(variable *to, variable from){
    variable ntemp = try(gsumvar(*to, from));
    gdelvar(to);
    gmovevar(&ntemp, to);
    
    return noerropt;
}

option __galc_init(){
    gpa = ((AbstractAllocator*)try(global.get(".absa")).data)->real;
    return noerropt;
}

void __galc_end(){
    ;// discard(__alc_gpa_end(gpa));
}
#endif
#define GALLC_HEADER
