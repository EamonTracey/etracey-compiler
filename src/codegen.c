#include <stdio.h>

#include "codegen.h"

extern FILE *codegen_out;

void codegen_funccall(const char *s) {
    fprintf(codegen_out, "    pushq %%r10\n");
    fprintf(codegen_out, "    pushq %%r11\n");
    fprintf(codegen_out, "    call %s\n", s);
    fprintf(codegen_out, "    popq %%r11\n");
    fprintf(codegen_out, "    popq %%r10\n");
}
