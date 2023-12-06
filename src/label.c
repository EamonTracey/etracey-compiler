#include <stdio.h>
#include <stdlib.h>

#include "label.h"

int label_create() {
    static int label = 0;

    return label++;
}

const char *label_name(int label) {
    char *name = (char *)malloc(16 * sizeof(char));
    snprintf(name, 16, ".L%d", label);
    return name;
}
