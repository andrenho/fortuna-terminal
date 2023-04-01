#include <assert.h>
#include <string.h>

#include <ft-client.h>

static char buffers[16][17] = { 0 };
static size_t cbuf = 0;

void* my_data = &cbuf;

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    assert(bufsz <= 16);
    assert(data == my_data);

    memset(buffers[cbuf], 0, sizeof(buffers[cbuf]));
    strncpy(buffers[cbuf++], buf, bufsz);

    return 0;
}

int main()
{
    FTClient ft;
    assert(ftclient_init(&ft, write_cb, NULL, my_data, 16) == 0);

#if 0
    cbuf = 0;
    assert(ft_reset_terminal(&ft) == 0);
    assert(strcmp("\e*r", buffers[0]) == 0);

    cbuf = 0;
    assert(ft_enable_vsync(&ft, true) == 0);
    assert(strcmp("\e*1V", buffers[0]) == 0);
#endif

    cbuf = 0;
    assert(ft_sprite_4(&ft, 10000, -50, 30, true, false, true, 8, 0) == 0);
    assert(strcmp("\e*10000;-50;30;1", buffers[0]) == 0);
    assert(strcmp("0;1;8;0", buffers[1]) == 0);
}