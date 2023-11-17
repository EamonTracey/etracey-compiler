#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "param_list.h"
#include "type.h"

struct type *type_create(type_t kind, struct type *subtype, struct param_list *params, struct expr *size) {
    struct type *type = (struct type *)malloc(sizeof(struct type));

    if (type == NULL)
        return NULL;

    type->kind = kind;
    type->subtype = subtype;
    type->params = params;
    type->size = size;

    return type;
}

void type_print(struct type *t) {
    if (t == NULL)
        return;

    switch (t->kind) {
    case TYPE_VOID:
        fprintf(stdout, "void");
        break;
    case TYPE_BOOLEAN:
        fprintf(stdout, "boolean");
        break;
    case TYPE_CHARACTER:
        fprintf(stdout, "char");
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
        fprintf(stdout, "array [");
        if (t->size != NULL)
            expr_print(t->size, 0);
        fprintf(stdout, "] ");
        type_print(t->subtype);
        break;
    case TYPE_FUNCTION:
        fprintf(stdout, "function ");
        type_print(t->subtype);
        fprintf(stdout, " ");
        fprintf(stdout, "(");
        param_list_print(t->params);
        fprintf(stdout, ")");
        break;
    }
}

int type_equals(struct type *t1, struct type *t2) {
    if ((t1 == NULL && t2 != NULL) || (t1 != NULL && t2 == NULL))
        return 0;
    if (t1 == NULL && t2 == NULL)
        return 1;

    if (t1->kind != t2->kind)
        return 0;

    if (t1->kind == TYPE_ARRAY) {
        /* 
         * mismatching sizes.
         * I take a very conservative approach here
         * by only comparing array sizes if both arrays
         * have an integer literal size.
         */
        if (t1->size && t2->size && t1->size->kind == EXPR_INTEGERLIT && t2->size->kind == EXPR_INTEGERLIT) {
            if (t1->size->literal_value != t2->size->literal_value)
                return 0;
        }
        return type_equals(t1->subtype, t2->subtype);
    }

    if (t1->kind == TYPE_FUNCTION) {
        return type_equals(t1->subtype, t2->subtype) && type_param_equals(t1->params, t2->params);
    }

    return 1;
}

int type_param_equals(struct param_list *p1, struct param_list *p2) {
    if (p1 == NULL && p2 == NULL)
        return 1;
    if (p1 == NULL && p2 != NULL)
        return 0;
    if (p1 != NULL && p2 == NULL)
        return 0;
    if (!type_equals(p1->type, p2->type))
        return 0;

    return type_param_equals(p1->next, p2->next);
}

int type_is_atomic(struct type *t) {
    return t->kind == TYPE_INTEGER
        || t->kind == TYPE_FLOAT
        || t->kind == TYPE_CHARACTER
        || t->kind == TYPE_STRING
        || t->kind == TYPE_BOOLEAN;
}
