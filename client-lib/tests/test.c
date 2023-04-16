#include <assert.h>
#include <string.h>

#include <ftclient.h>
#include <ftclient-png.h>

static char buffers[16][FT_RECOMMENDED_BUFSZ] = { 0 };
static size_t cbuf = 0;

static char read_poll[256];
static size_t rbuf = 0;

void* my_data = &cbuf;

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    assert(data == my_data);

    memset(buffers[cbuf], 0, sizeof(buffers[cbuf]));
    strncpy(buffers[cbuf++], buf, bufsz);

    return 0;
}

static int read_cb(char* buf, size_t bufsz, void* data)
{
    assert(bufsz <= 16);
    assert(data == my_data);

    memcpy(buf, &read_poll[rbuf], bufsz);
    rbuf += bufsz;

    if (strlen(buf) == 0)
        return 0;

    return (int) bufsz;
}

int main()
{
    FTClient ft;
    assert(ftclient_init(&ft, write_cb, read_cb, NULL, my_data, 16) == 0);

    // test writes

    cbuf = 0;
    assert(ft_reset_terminal(&ft) == 0);
    assert(strcmp("\e*r", buffers[0]) == 0);

    cbuf = 0;
    assert(ft_enable_vsync(&ft, true) == 0);
    assert(strcmp("\e*1V", buffers[0]) == 0);

    cbuf = 0;
    assert(ft_sprite_4(&ft, 10000, -50, 30, true, false, true, 8, 0) == 0);
    assert(strcmp("\e*10000;-50;30;", buffers[0]) == 0);
    assert(strcmp("1;0;1;8;0S", buffers[1]) == 0);

    cbuf = 0;
    assert(ft_sprite_4(&ft, 8, 1, 1, true, true, true, 1, 8) == 0);
    assert(strcmp("\e*8;$6,1;8S", buffers[0]) == 0);

    cbuf = 0;
    assert(ft_print(&ft, "0123456789ABCDEFGHIJKL") == 0);
    assert(strcmp("0123456789ABCDEF", buffers[0]) == 0);
    assert(strcmp("GHIJKL", buffers[1]) == 0);

    // test reads

    FT_Event e;
    strcpy(read_poll, "\e#1;2;3v\e#0;50;-33B");

    assert(ft_poll_event(&ft, &e) != 0);
    assert(e.type == FTE_VERSION);
    assert(e.version.major == 1 && e.version.minor == 2 && e.version.patch == 3);

    assert(ft_poll_event(&ft, &e) != 0);
    assert(e.type == FTE_MOUSE_PRESS);
    assert(e.mouse.button == 0 && e.mouse.pos_x == 50 && e.mouse.pos_y == -33);

    assert(ft_poll_event(&ft, &e) == 0);

    // test PNG

    assert(ftclient_init(&ft, write_cb, read_cb, NULL, my_data, FT_RECOMMENDED_BUFSZ) == 0);
    cbuf = 0;
    assert(ft_image_load(&ft, "test.png", NULL, 0) == 0);
    assert(strcmp("\e*0;7;$17,0;19;$238,0i", buffers[0]) == 0);
    assert(strcmp("\e*1;7;$238,0;21;$17,0i", buffers[1]) == 0);
}