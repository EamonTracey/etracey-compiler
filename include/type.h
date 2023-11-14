#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"

typedef enum {
    TYPE_VOID,
    TYPE_BOOLEAN,
    TYPE_CHARACTER,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
} type_t;

struct type {
    type_t kind;
    struct type *subtype;
    struct param_list *params;

    // Relevant exclusively to arrays.
    struct expr *size;
};

struct type *type_create(type_t kind, struct type *subtype, struct param_list *params, struct expr *size);
void type_print(struct type *t);
int type_equals(struct type *t1, struct type *t2);
int type_param_equals(struct param_list *p1, struct param_list *p2);

#endif
