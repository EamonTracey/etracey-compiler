#ifndef ENCODE_H
#define ENCODE_H

int integer_decode(const char *, long int *);
int float_decode(const char *, double *);
int string_decode(const char *, char *);
int string_encode(const char *, char *);

int hex_to_val(int hex);
int val_to_hex(int val);

#endif