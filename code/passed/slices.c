#include <mds/modules.h>
#include <mds/core_impl.h>

int carray[] = {1,2, 3, 56, 67, 123, 43534, 7645,74,423,4235, 3};
size_t clen = sizeof(carray) / sizeof(carray[0]);

void arrprint(array *arr){
    std.array.foreach(arr, lambda(variable *vr, size_t inx){
        std.io.term.println("[%zu] element: %zu", inx, *(int*)vr->data);
    return 1;});
}

$test(slices_basic){
    var slm = std.mem.slices;
    var arm = std.array;

    array *arr = try(arm.fromc(carray, sizeof(int), clen)).data;

    // slice
    Slice slice = slm.slice(arr->elements, sizeof(variable), 1, 4);
    std.io.term.println("Slice info: size: %zu el_size: %zu", slice.slice_size, slice.el_size);
    {
        array *slice_arr = try(slm.slice_as_array(slice)).data;
        arrprint(slice_arr);

        arm.free(slice_arr);
        slm.free_slice(&slice);
    }

    arm.free(arr);
    return noerropt;
}
