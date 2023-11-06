#include <stdlib.h>
#include <stdio.h>

#include "param_list.h"
#include "scope.h"
#include "type.h"

struct param_list *param_list_create(char *name, struct type *type, struct param_list *next) {
    struct param_list *param_list = (struct param_list *)malloc(sizeof(struct param_list));

    param_list->name = name;
    param_list->type = type;
    param_list->next = next;

    param_list->symbol = NULL;

    return param_list;
}

void param_list_print(struct param_list *a) {
    if (a == NULL)
        return;

    fprintf(stdout, "%s: ", a->name);
    type_print(a->type);

    if (a->next != NULL) {
        fprintf(stdout, ", ");
        param_list_print(a->next);
    }
}

void param_list_resolve(struct param_list *a) {
    if (a == NULL)
        return;

    struct symbol *s = scope_lookup_current(a->name);
    if (s != NULL) {
        fprintf(stdout, "error: parameter %s was previously declared as type ", a->name);
        type_print(s->type);
        fprintf(stdout, ".\n");
        return;
    } else {
        a->symbol = symbol_create(SYMBOL_PARAM, a->type, a->name);
        scope_bind(a->name, a->symbol);
    }

    param_list_resolve(a->next);
}
