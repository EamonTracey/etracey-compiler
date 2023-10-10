#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "constants.h"
#include "encode.h"

int integer_decode(const char *eint, long int *bmint) {
   *bmint = strtol(eint, NULL, 10); 

    if (errno == ERANGE)
        return *bmint == 0 ? ERROR_INTENC_UNDERFLOW : ERROR_INTENC_OVERFLOW;

   return 0;
}

int float_decode(const char *efloat, double *bmfloat) {
   *bmfloat = strtod(efloat, NULL); 

   if (errno == ERANGE)
       return *bmfloat == 0 ? ERROR_FLOATENC_UNDERFLOW : ERROR_FLOATENC_OVERFLOW;

   return 0;
}

int char_decode(const char *echar, char *bmchar) {
    // Ensure the character begins with a single quotation mark.
    if (*echar++ != '\'')
        return ERROR_CHARENC_BQUOTE;

    // Character literals must contain only a printable.
    // Non-printable characters must use backslash codes.
    if (*echar < 32 || *echar > 126)
        return ERROR_CHARENC_PRINTABLE;

    // If the character is a backslash, parse backslash code.
    // Otherwise, copy character value.
    if (*echar == '\\') {
        ++echar;
        switch (*echar++) {
        case 'a':
            *bmchar = 7;
            break;
        case 'b':
            *bmchar = 8;
            break;
        case 'e':
            *bmchar = 27;
            break;
        case 'f':
            *bmchar = 12;
            break;
        case 'n':
            *bmchar = 10;
            break;
        case 'r':
            *bmchar = 13;
            break;
        case 't':
            *bmchar = 9;
            break;
        case 'v':
            *bmchar = 11;
            break;
        case '\\':
            *bmchar = 92;
            break;
        case '\'':
            *bmchar = 39;
            break;
        case '"':
            *bmchar = 34;
            break;
        case '0':
            if (*echar++ != 'x')
                return ERROR_CHARENC_HEX;
            int val1 = hex_to_val(*echar++);
            if (val1 == -1)
                return ERROR_CHARENC_HEX;
            int val2 = hex_to_val(*echar++);
            if (val2 == -1)
                return ERROR_CHARENC_HEX;
            char val = val1 * 16 + val2;
            *bmchar = val;
            break;
        default:
            return ERROR_CHARENC_CODE;
        }
    } else {
        *bmchar = *echar++;
    }

    // The character after the ending quotation mark must be NUL.
    if (*echar++ != '\'')
        return ERROR_CHARENC_EQUOTE;

    if (*echar != '\0')
        return ERROR_CHARENC_TRAIL;

    return 0;
}

int string_decode(const char *es, char *s) {
    // Ensure the string begins with a quotation mark.
    if (*es++ != '"')
        return ERROR_STRENC_BQUOTE;

    // Iterate through each character until an ending quotation mark.
    int length = 0;
    while (*es != '"') {
        // Strings must not be longer than 255 characters.
        if (++length > 255)
            return ERROR_STRENC_LENGTH;

        // String literals must contain only printable characters.
        // Non-printable characters must use backslash codes.
        if (*es < 32 || *es > 126)
            return ERROR_STRENC_PRINTABLE;

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
                    return ERROR_STRENC_HEX;
                int val1 = hex_to_val(*es++);
                if (val1 == -1)
                    return ERROR_STRENC_HEX;
                int val2 = hex_to_val(*es++);
                if (val2 == -1)
                    return ERROR_STRENC_HEX;
                char val = val1 * 16 + val2;
                *s++ = val;
                break;
            default:
                return ERROR_STRENC_CODE;
            }
        } else {
            *s++ = *es++;
        }

    }

    // The character after the ending quotation mark must be NUL.
    if (*++es != '\0')
        return ERROR_STRENC_TRAIL;

    *s = '\0';

    return 0;
}

int string_encode(const char *s, char *es) {
    *es++ = '"';

    int length = 0;
    while (*s != '\0') {
        if (++length > 255)
            return ERROR_STRENC_LENGTH;

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
            int val1 = *s >> 4;
            int val2 = *s & 0x0f;
            *es++ = (char)val_to_hex(val1);
            *es++ = (char)val_to_hex(val2);
        }

        ++s;
    }
    
    *es++ = '"';
    *es = '\0';

    return 0;
}

int hex_to_val(int hex) {
    if ('0' <= hex && hex <= '9')
        return hex - 48;
    else if ('A' <= hex && hex <= 'F')
        return hex - 55;
    else if ('a' <= hex && hex <= 'f')
        return hex - 87;
    else
        return -1;
}

int val_to_hex(int val) {
    if (0 <= val && val <= 9)
        return val + 48;
    else if (10 <= val && val <= 15)
        return val + 87;
    else
        return -1;
}
