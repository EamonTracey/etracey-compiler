#ifndef SCRATCH_H
#define SCRATCH_H

int scratch_alloc();
void scratch_free(int reg);
const char *scratch_name(int reg);

#endif
