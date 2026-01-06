#include <mds/core_impl.h>
#include <mds/modules.h>

$test(tokenize){
    char *data = "Hello world spaced!";
    array *arr = try(std.mem.tokenizer.tokenizeString(data, ' ')).data;
    for (size_t i = 0; i < arr->len; i++){
        Slice *slice = arr->elements[i].data;
        std.io.term.print("%.*s\n", slice->size, slice->data);
    }

    return noerropt;
}
