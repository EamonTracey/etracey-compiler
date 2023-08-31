#include <stdio.h>

#include "encode.h"

char hex_to_val(char hex) {
    if ('0' <= hex && hex <= '9')
        return hex - 48;
    else if ('A' <= hex && hex <= 'F')
        return hex - 55;
    else if ('a' <= hex && hex <= 'f')
        return hex - 87;
    else
        return -1;
}

char val_to_hex(char hex) {
    if (0 <= hex && hex <= 9)
        return hex + 48;
    else if (10 <= hex && hex <= 15)
        return hex + 87;
    else
        return -1;
}

int string_decode(const char *es, char *s) {
    // Ensure the string begins with a quotation mark.
    if (*es++ != '"')
        return ENC_BQUOTE;

    // Iterate through each character until an ending quotation mark.
    int length = 0;
    while (*es != '"') {
        // Strings must not be longer than 255 characters.
        if (++length > 255)
            return ENC_LENGTH;

        // String literals must contain only printable characters.
        // Non-printable characters must use backslash codes.
        if (*es < 32 || *es > 126)
            return ENC_PRINTABLE;

        // If the character is a backslash, parse backslash code.
        // Otherwise, copy character value.
        if (*es == '\\') {
            ++es;
            switch (*es++) {
            case 'a':
                *s++ = 7;
                break;
            case 'b':
                *s++ = 8;
                break;
            case 'e':
                *s++ = 27;
                break;
            case 'f':
                *s++ = 12;
                break;
            case 'n':
                *s++ = 10;
                break;
            case 'r':
                *s++ = 13;
                break;
            case 't':
                *s++ = 9;
                break;
            case 'v':
                *s++ = 11;
                break;
            case '\\':
                *s++ = 92;
                break;
            case '\'':
                *s++ = 39;
                break;
            case '"':
                *s++ = 34;
                break;
            case '0':
                if (*es++ != 'x')
                    return ENC_HEX;
                char val1 = hex_to_val(*es++);
                if (val1 == -1)
                    return ENC_HEX;
                char val2 = hex_to_val(*es++);
                if (val2 == -1)
                    return ENC_HEX;
                char val = val1 * 16 + val2;
                *s++ = val;
                break;
            default:
                return ENC_CODE;
            }
        } else {
            *s++ = *es++;
        }

    }

    // The character after the ending quotation mark must be NUL.
    if (*++es != '\0')
        return ENC_TRAIL;

    *s = '\0';

    return 0;
}

int string_encode(const char *s, char *es) {
    *es++ = '"';

    int length = 0;
    while (*s != '\0') {
        if (++length > 255)
            return ENC_LENGTH;

        if (*s == 7) {
            *es++ = '\\';
            *es++ = 'a';
        } else if (*s == 8) {
            *es++ = '\\';
            *es++ = 'b';
        } else if (*s == 27) {
            *es++ = '\\';
            *es++ = 'e';
        } else if (*s == 12) {
            *es++ = '\\';
            *es++ = 'f';
        } else if (*s == 10) {
            *es++ = '\\';
            *es++ = 'n';
        } else if (*s == 13) {
            *es++ = '\\';
            *es++ = 'r';
        } else if (*s == 9) {
            *es++ = '\\';
            *es++ = 't';
        } else if (*s == 11) {
            *es++ = '\\';
            *es++ = 'v';
        } else if (*s == 92) {
            *es++ = '\\';
            *es++ = '\\';
        } else if (*s == 34) {
            *es++ = '\\';
            *es++ = '"';
        } else if (32 <= *s && *s <= 126)
            *es++ = *s;
        else {
            *es++ = '\\';
            *es++ = '0';
            *es++ = 'x';
            char val1 = (unsigned char)*s >> 4;
            char val2 = *s & 0x0f;
            *es++ = val_to_hex(val1);
            *es++ = val_to_hex(val2);
        }

        ++s;
    }
    
    *es++ = '"';
    *es = '\0';

    return 0;
}
