#ifndef ERROR_H
#define ERROR_H

#define ERROR_INTENC_UNDERFLOW 1
#define ERROR_INTENC_OVERFLOW 2

#define ERROR_FLOATENC_UNDERFLOW 1
#define ERROR_FLOATENC_OVERFLOW 2

#define ERROR_STRENC_BQUOTE 1
#define ERROR_STRENC_PRINTABLE 2
#define ERROR_STRENC_LENGTH 3
#define ERROR_STRENC_TRAIL 4
#define ERROR_STRENC_HEX 5
#define ERROR_STRENC_CODE 6

extern const char *intencerr[];
extern const char *floatencerr[];
extern const char *strencerr[];

#endif
