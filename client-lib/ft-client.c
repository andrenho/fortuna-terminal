#include "ft-client.h"

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
    buf[2] = cmd;
    return ft->write_cb(buf, 3, ft->data);
}

int reset_terminal(FTClient* ft)
{
    return write_request(ft, 'r', NULL, 0);
}