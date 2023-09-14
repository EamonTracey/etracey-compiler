#include "error.h"

const char *intencerr[] = {
    "",
    "integer literal %s causes underflow.",
    "integer literal %s causes overflow."
};

const char *floatencerr[] = {
    "",
    "float literal %s causes underflow.",
    "float literal %s causes overflow."
};

const char *strencerr[] = {
    "",
    "string literal must begin with quotation mark.",
    "string literal must contain only printable characters and backslash codes.",
    "string must be at most 255 characters.",
    "string literal must end with quotation mark.",
    "hex backslash code must be of valid form \\0xHH.",
    "error: invalid backslash code, use only a, b, e, f, n, r, t, v, \\, ', \", 0xHH."
};
