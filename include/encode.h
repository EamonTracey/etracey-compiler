#ifndef ENCODE_H
#define ENCODE_H

#define ENC_BQUOTE 1
#define ENC_PRINTABLE 2
#define ENC_LENGTH 3
#define ENC_TRAIL 4
#define ENC_HEX 5
#define ENC_HEXPRINT 6

int string_decode(const char *es, char *s);
int string_encode(const char *s, char *es);

#endif
