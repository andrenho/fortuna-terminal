#include "ft-client.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

static int write_request(FTClient* ft, char cmd, int16_t* array, size_t array_sz)
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
            buf[0] = '\0';
        }
        strcpy(&buf[i], buf2);
        i += sz;
        if (j != (array_sz - 1)) {
            buf[i++] = ';';
            buf[i] = '\0';
        }
    }

    buf[i++] = cmd;
    return ft->write_cb(buf, i, ft->data);
}

int ft_print(FTClient* ft, const char* fmt, ...)
{
    char buf[ft->bufsz];

    va_list ap;
    va_start(ap, fmt);
    int sz = vsnprintf(buf, ft->bufsz, fmt, ap);
    va_end(ap);

    if (sz < 0)
        return -1;

    return ft->write_cb(buf, sz, ft->data);
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
    int16_t array = { enable };
    return write_request(ft, 'V', &array, 1);
}

int ft_request_version(FTClient* ft)
{
    return write_request(ft, 'v', NULL, 0);
}

int ft_mouse_enable(FTClient* ft, bool enable)
{
    int16_t array = { enable };
    return write_request(ft, 'm', &array, 1);
}

int ft_mouse_move_enable(FTClient* ft, bool enable)
{
    int16_t array = { enable };
    return write_request(ft, 'n', &array, 1);
}

int ft_joystick_emulation(FTClient* ft, bool enable)
{
    int16_t array = { enable };
    return write_request(ft, 'j', &array, 1);
}

int ft_graphics(FTClient* ft, bool enable)
{
    int16_t array = { enable };
    return write_request(ft, 'g', &array, 1);
}

int ft_palette(FTClient* ft, FTColor colors[FT_N_COLORS])
{
    int16_t array[FT_N_COLORS * 3];
    for (size_t i = 0; i < FT_N_COLORS; ++i) {
        array[i * 3 + 0] = colors[i].r;
        array[i * 3 + 1] = colors[i].g;
        array[i * 3 + 2] = colors[i].b;
    }
    return write_request(ft, 'P', array, FT_N_COLORS * 3);
}

int ft_image(FTClient* ft, int16_t index, int16_t transparent_color, const uint8_t bytes[256])
{
    int16_t array[258];
    array[0] = index;
    array[1] = transparent_color;
    for (size_t i = 0; i < 256; ++i)
        array[i+2] = bytes[i];
    return write_request(ft, 'i', array, 258);
}

#ifdef FT_PNG_SUPPORT
#include <png.h>
#include <setjmp.h>

int ft_image_load(FTClient* ft, const char* filename, char* error, size_t err_sz)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        snprintf(error, err_sz, "Error opening PNG file.");
        return -1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    setjmp(png_jmpbuf(png_ptr));
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);


}
#endif

int ft_bg_color(FTClient* ft, int16_t color)
{
    return write_request(ft, 'B', &color, 1);
}

int ft_enable_layer(FTClient* ft, int16_t layer, bool enable)
{
    int16_t array[2] = { layer, enable };
    return write_request(ft, 'L', array, 2);
}

int ft_map_pos(FTClient* ft, int16_t layer, int16_t map, int16_t pos_x, int16_t pos_y)
{
    int16_t array[4] = { layer, map, pos_x, pos_y };
    return write_request(ft, 'M', array, 4);
}

int ft_sprite_0(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y)
{
    int16_t array[3] = { sprite_n, pos_x, pos_y };
    return write_request(ft, 'S', array, 3);
}

int ft_sprite_1(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible)
{
    int16_t array[4] = { sprite_n, pos_x, pos_y, visible };
    return write_request(ft, 'S', array, 4);
}

int ft_sprite_2(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v)
{
    int16_t array[6] = { sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v };
    return write_request(ft, 'S', array, 6);
}

int ft_sprite_3(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v, int16_t z_order)
{
    int16_t array[7] = { sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v, z_order };
    return write_request(ft, 'S', array, 7);
}

int ft_sprite_4(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v, int16_t z_order, int16_t image_idx)
{
    int16_t array[8] = { sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v, z_order, image_idx };
    return write_request(ft, 'S', array, 8);
}

int ft_subscribe_collisions(FTClient* ft, int16_t sprite_a, int16_t sprite_b)
{
    int16_t array[2] = { sprite_a, sprite_b };
    return write_request(ft, 'C', array, 2);
}

int ft_unsubscribe_collisions(FTClient* ft, int16_t sprite_a, int16_t sprite_b)
{
    int16_t array[2] = { sprite_a, sprite_b };
    return write_request(ft, 'U', array, 2);
}

int ft_unsubscribe_all_collisions(FTClient* ft)
{
    return write_request(ft, 'C', NULL, 0);
}

void parse_event(char cmd, const int16_t arr[], int arr_sz, FT_Event *event)
{
    switch (cmd) {
        case 'v':
            event->type = FTE_VERSION;
            event->version.major = arr[0];
            event->version.minor = arr[1];
            event->version.patch = arr[2];
            break;
        case 'B':
            event->type = FTE_MOUSE_PRESS;
            event->mouse.button = arr[0];
            event->mouse.pos_x = arr[1];
            event->mouse.pos_y = arr[2];
            break;
        case 'R':
            event->type = FTE_MOUSE_RELEASE;
            event->mouse.button = arr[0];
            event->mouse.pos_x = arr[1];
            event->mouse.pos_y = arr[2];
            break;
        case 'M':
            event->type = FTE_MOUSE_MOVE;
            event->mouse.button = arr[0];
            event->mouse.pos_x = arr[1];
            event->mouse.pos_y = arr[2];
            break;
        case 'J':
            event->type = FTE_JOY_PRESS;
            event->joy = arr[0];
            break;
        case 'K':
            event->type = FTE_JOY_RELEASE;
            event->joy = arr[0];
            break;
        case 'c':
            event->type = FTE_COLLISION;
            event->collision.type = arr[0] == 0 ? FTEC_COLLISION : FTEC_SEPARATION;
            event->collision.sprite_a = arr[1];
            event->collision.sprite_b = arr[2];
            break;
        default:
            event->type = FTE_INVALID_ESCAPE;
    }
}

int ft_poll_event(FTClient* ft, FT_Event* event)
{
#define GETCHAR { if (ft->read_cb(&c, 1, ft->data) <= 0) return 0; }

    int16_t arr[ft->bufsz / sizeof(int16_t)];
    char cmd;

    int i = 0;
    char c;
    if (ft->read_cb(&c, 1, ft->data) <= 0)
        return 0;

    if (c == (char) 0xfe) {
        event->type = FTE_VSYNC;
        return 1;
    } else if (c != '\e') {
        event->type = FTE_KEY_PRESS;
        event->key = c;
        return 1;
    }

    if (ft->read_cb(&c, 1, ft->data) <= 0)
        return 0;
    if (c != '#')
        return 0;

    int16_t num = 0;
    size_t count = 0;

    if (ft->read_cb(&c, 1, ft->data) <= 0)
        return 0;

    while (true) {
        if (c == ';') {
            arr[i++] = num;
            num = 0;
        } else if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (isalpha(c)) {
            cmd = c;
            arr[i] = num;
            break;
        }
        if (ft->read_cb(&c, 1, ft->data) <= 0)
            return 0;
        ++count;
        if (count >= 16)
            return 0;
    }

    parse_event(cmd, arr, i, event);
    return 1;
}