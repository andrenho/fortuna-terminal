#include "ftclient-avr.h"

#include <stdio.h>

int ft_print_P(FTClient* ft, PGM_P str)
{
    size_t len = strlen_P(str);
    size_t i = 0;

    while (i < len) {
        size_t bytes_to_write = ((i + ft->bufsz) > len) ? len - i : ft->bufsz;
        char buf[bytes_to_write];
        memcpy_P(buf, str + i, bytes_to_write);
        int r = ft->write_cb(buf, bytes_to_write, ft->data);
        if (r < 0)
            return r;
        i += ft->bufsz;
    }

    return 0;
}

int ft_printf_P(FTClient* ft, PGM_P fmt, ...)
{
    char buf[ft->bufsz];

    va_list ap;
    va_start(ap, fmt);
    int sz = vsnprintf_P(buf, ft->bufsz, fmt, ap);
    va_end(ap);

    if (sz < 0)
        return -1;

    return ft->write_cb(buf, sz, ft->data);
}