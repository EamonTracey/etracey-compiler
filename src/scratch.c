#include <stdio.h>
#include <stdlib.h>

#include "scratch.h"

/* We have 6 scratch registers. */
static int scratch_registers[6] = { 0, 0, 0, 0, 0, 0 };

static const char *scratch_names[6] = {
    "%rbx",
    "%r11",
    "%r12",
    "%r13",
    "%r14",
    "%r15"
};

int scratch_alloc() {
    int i;
    for (i = 0; i < 6; ++i)
        if (scratch_registers[i] == 0) {
            scratch_registers[i] = 1;
            break;
        }

    if (i == 6) {
        fprintf(stdout, "codegen error: exhausted scratch registers.\n");
        exit(1);
    }

    return i;
}

void scratch_free(int reg) {
    scratch_registers[reg] = 0;
}

const char *scratch_name(int reg) {
    return scratch_names[reg];
}
