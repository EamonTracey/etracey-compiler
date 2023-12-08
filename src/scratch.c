#include <stdio.h>
#include <stdlib.h>

#include "scratch.h"

/* We have 7 scratch registers. */
static int scratch_registers[7] = { 0, 0, 0, 0, 0, 0, 0 };

static const char *scratch_names[7] = {
    "%rbx",
    "%r10",
    "%r11",
    "%r12",
    "%r13",
    "%r14",
    "%r15"
};

int scratch_alloc() {
    int i;
    for (i = 0; i < 7; ++i)
        if (scratch_registers[i] == 0) {
            scratch_registers[i] = 1;
            return i;
        }

    fprintf(stdout, "codegen error: exhausted scratch registers.\n");
    exit(1);

}

void scratch_free(int reg) {
    scratch_registers[reg] = 0;
}

int scratch_check(int reg) {
    return scratch_registers[reg];
}

void scratch_set(int reg, int val) {
    scratch_registers[reg] = val;
}

const char *scratch_name(int reg) {
    return scratch_names[reg];
}

/* We have 8 scratch float registers. */
static int scratch_float_registers[5] = { 0, 0, 0, 0, 0 };

static const char *scratch_float_names[5] = {
    "%xmm3",
    "%xmm4",
    "%xmm5",
    "%xmm6",
    "%xmm7"
};

int scratch_float_alloc() {
    int i;
    for (i = 0; i < 5; ++i)
        if (scratch_float_registers[i] == 0) {
            scratch_float_registers[i] = 1;
            return i;
        }

    fprintf(stdout, "codegen error: exhausted scratch float registers.\n");
    exit(1);
}

void scratch_float_free(int reg) {
    scratch_float_registers[reg] = 0;
}

const char *scratch_float_name(int reg) {
    return scratch_float_names[reg];
}
