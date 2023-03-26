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
                  int (*read_cb)(char* buf, size_t bufsz, void* data),  // nullable
                  void* data,                                           // nullable
                  size_t bufsz);                                        // use FT_RECOMMENDED_BUFSZ, except for microcontrollers

int ft_reset_terminal(FTClient* ft);

#endif //FT_CLIENT_H_
