#include "ft-client.h"

#include <stdio.h>
#include <string.h>

int ftclient_init(FTClient* ft,
                  int (*write_cb)(const char* buf, size_t bufsz, void* data),
                  int (*read_cb)(char* buf, size_t bufsz, void* data),
                  void*  data,
                  size_t bufsz)
{
    ft->write_cb = write_cb;
    ft->read_cb = read_cb;
    ft->data = data;
    ft->bufsz = bufsz;
    return 0;
}

static int write_request(FTClient* ft, char cmd, int* array, size_t array_sz)
{
    char buf[ft->bufsz];
    buf[0] = '\e';
    buf[1] = '*';
    size_t i = 2;

    for (size_t j = 0; j < array_sz; ++j) {
        char buf2[8];
        int sz = snprintf(buf2, sizeof buf2, "%d", array[j]);
        if (i + sz > ft->bufsz) {
            int r = ft->write_cb(buf, i, ft->data);
            if (r < 0)
                return r;
            i = 0;
        }
        strcat(&buf[i], buf2);
        i += sz;
        if (j != (array_sz - 1))
            buf[i++] = ';';
    }

    buf[i++] = cmd;
    return ft->write_cb(buf, i, ft->data);
}

int ft_reset_terminal(FTClient* ft)
{
    return write_request(ft, 'r', NULL, 0);
}

int ft_reset_computer(FTClient* ft)
{
    return write_request(ft, 'x', NULL, 0);
}

int ft_enable_vsync(FTClient* ft, bool enable)
{
    int array = { enable };
    return write_request(ft, 'V', &array, 1);
}

int ft_request_version(FTClient* ft)
{
    return write_request(ft, 'v', NULL, 0);
}