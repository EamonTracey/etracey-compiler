#include <stdio.h>
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
    switch (t->kind) {
    case TYPE_VOID:
        fprintf(stdout, "void");
        break;
    case TYPE_BOOLEAN:
        fprintf(stdout, "boolean");
        break;
    case TYPE_CHARACTER:
        fprintf(stdout, "character");
        break;
    case TYPE_INTEGER:
        fprintf(stdout, "integer");
        break;
    case TYPE_FLOAT:
        fprintf(stdout, "float");
        break;
    case TYPE_STRING:
        fprintf(stdout, "string");
        break;
    case TYPE_ARRAY:
        fprintf(stdout, "array [] ");
        type_print(t->subtype);
        break;
    case TYPE_FUNCTION:
        fprintf(stdout, "function ");
        type_print(t->subtype);
        fprintf(stdout, " ()");
        break;
    }
}
