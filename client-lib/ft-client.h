#ifndef FT_CLIENT_H_
#define FT_CLIENT_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct FTClient {
    int    (*write_cb)(const char* buf, size_t bufsz, void* data);
    int    (*read_cb)(char* buf, size_t bufsz, void* data);
    void*  data;
    size_t bufsz;
} FTClient;

#define FT_RECOMMENDED_BUFSZ 1024

int ftclient_init(FTClient* ft_client,
                  int (*write_cb)(const char* buf, size_t bufsz, void* data),
                  int (*read_cb)(char* buf, size_t bufsz, void* data),
                  void* data,
                  size_t bufsz);  // use FT_RECOMMENDED_BUFSZ, except for microcontrollers

int ft_print(FTClient* ft, const char* fmt, ...);

int ft_reset_terminal(FTClient* ft);
int ft_reset_computer(FTClient* ft);
int ft_enable_vsync(FTClient* ft, bool enable);
int ft_request_version(FTClient* ft);

int ft_mouse_enable(FTClient* ft, bool enable);
int ft_mouse_move_enable(FTClient* ft, bool enable);
int ft_joystick_emulation(FTClient* ft, bool enable);

#endif //FT_CLIENT_H_
