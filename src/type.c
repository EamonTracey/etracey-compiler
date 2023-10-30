#include <stdlib.h>

#include "param_list.h"
#include "type.h"

struct type *type_create(type_t kind, struct type *subtype, struct param_list *params) {
    struct type *type = (struct type *)malloc(sizeof(struct type));

    if (type == NULL)
        return NULL;

    type->kind = kind;
    type->subtype = subtype;
    type->params = params;

    return type;
}

void type_print(struct type *t) {
}
