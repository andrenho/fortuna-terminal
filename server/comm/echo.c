#include "echo.h"
#include "error/error.h"

#include <pthread.h>
#include <string.h>

static pthread_mutex_t mutex;

#define BUFFER_SZ 4096
uint8_t buffer_[BUFFER_SZ];
size_t  buffer_sz_ =0;

int echo_init()
{
    pthread_mutex_init(&mutex, NULL);
    return 0;
}

int echo_recv(uint8_t* byte)
{
    if (buffer_sz_ == 0)
        return ERR_NO_DATA;
    pthread_mutex_lock(&mutex);
    *byte = buffer_[--buffer_sz_];
    pthread_mutex_unlock(&mutex);
    return 0;
}

int echo_send(const uint8_t* data, size_t sz)
{
    pthread_mutex_lock(&mutex);
    memcpy(&buffer_[buffer_sz_], data, sz);
    buffer_sz_ += sz;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int echo_finalize()
{
    pthread_mutex_destroy(&mutex);
    return 0;
}