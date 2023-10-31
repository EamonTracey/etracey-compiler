#include <stdio.h>

void indent_print(int indent) {
    for (int i = 0; i < indent; ++i)
        fprintf(stdout, "    ");
}
