#include <stdlib.h>

#include "param_list.h"
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
}