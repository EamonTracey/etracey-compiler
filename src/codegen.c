#include <stdio.h>

#include "codegen.h"

void codegen_funccall(const char *s) {
    fprintf(stdout, "    pushq %%r10\n");
    fprintf(stdout, "    pushq %%r11\n");
    fprintf(stdout, "    call %s\n", s);
    fprintf(stdout, "    popq %%r11\n");
    fprintf(stdout, "    popq %%r10\n");
}
