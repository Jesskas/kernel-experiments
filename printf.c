#include "printf.h"
#define U32MAXSTRLEN 10 // strlen("2147483647")

int vsprintf(char* buf, const char* fmt, va_list args)
{
    char* str;

    unsigned int num;
    char* s;

    char digits[U32MAXSTRLEN]; 
    int digit_len;
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        fmt++;
        switch (*fmt) 
        {
            case 'c':
                num = (unsigned char)va_arg(args, int);
                *str++ = num;
                break;
            case 'd':
            case 'i':
                // linux kernel defaults 'i' to signed integers
                // absolute defaults here for now, signed, base 10, etc.

                // TODO: Heavy bug I can't trace the underlying cause for.
                // Theoretically, integers>0x80000000 in 'd'/'i' get scanned
                // and printed fine. However, for some reason, every time
                // a number with the top bit set is parsed, the remaining
                // va_arg returns are completely jumbled and in the wrong order.
                // Might need to look over some signed+unsigned notes...

                for (num = 0; num < 10; num++)
                    digits[num] = 0;
                digit_len = 0;

                num = va_arg(args, unsigned int);
                while (num) {
                    char digit = num % 10;
                    num /= 10;
                    digits[digit_len++] = digit;
                }

                while (digit_len) {
                    *str++ = digits[--digit_len] + '0';
                }
                break;
            case 'e':
            case 'E':
                // not implemented!
                break;
            case 'f':
                // not implemented!
                break;
            case 'g':
            case 'G':
                // not implemented!
                break;
            case 'o':
                // not implemented!
                break;
            case 'p':
                // not implemented!
                break;
            case 's':
                s = va_arg(args, char*);
                while (*s) {
                    *str++ = *s++;
                }
                break;
            case 'u':
                // not implemented!
                break;
            case 'x':
            case 'X':
                // absolute defaults here for now, signed, base 16, etc.
                // TODO: definitely signedness! anything above 0x80000000 fails
                for (num = 0; num < 10; num++)
                    digits[num] = 0;
                digit_len = 0;

                num = va_arg(args, unsigned int);
                //if (!num)
                //    num = va_arg(args, unsigned int);
                while (num) {
                    char digit = num % 16;
                    num /= 16;
                    digits[digit_len++] = digit;
                }

                while (digit_len) {
                    char digit = digits[--digit_len];
                    if (digit < 10)
                        *str++ = digit + '0';
                    else
                        *str++ = digit - 10 + (*fmt == 'x' ? 'a' : 'A');
                }
                break;
            default:
                // not implemented!
                break;
        }

    }

    return 0;
}
