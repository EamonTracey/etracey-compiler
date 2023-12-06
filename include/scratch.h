#ifndef SCRATCH_H
#define SCRATCH_H

int scratch_alloc();
void scratch_free(int reg);
int scratch_check(int reg);
void scratch_set(int reg, int val);
const char *scratch_name(int reg);

#endif
