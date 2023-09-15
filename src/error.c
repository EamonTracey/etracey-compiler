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

const char *charencerr[] = {
    "",
    "character literal must begin with single quotation mark.",
    "character literal must end with single quotation mark.",
    "character literal must contain only a printable character backslash code.",
    "string literal must end with single quotation mark.",
    "hex backslash code must be of valid form \\0xHH.",
    "invalid backslash code, use only a, b, e, f, n, r, t, v, \\, ', \", 0xHH."
};

const char *strencerr[] = {
    "",
    "string literal must begin with quotation mark.",
    "string literal must contain only printable characters and backslash codes.",
    "string must be at most 255 characters.",
    "string literal must end with quotation mark.",
    "hex backslash code must be of valid form \\0xHH.",
    "invalid backslash code, use only a, b, e, f, n, r, t, v, \\, ', \", 0xHH."
};

const char *scanerr[] = {
    "",
    "identifier must not exceed 255 characters.",
    "invalid token %s."
};