#include "echo.h"
#include "error/error.h"

#include <pthread.h>
#include <string.h>
#include <unistd.h>

static pthread_mutex_t mutex;

#define BUFFER_SZ 4096
uint8_t buffer_[BUFFER_SZ];
size_t  buffer_sz_ =0;

FT_Result echo_init()
{
    pthread_mutex_init(&mutex, NULL);
    return FT_OK;
}

FT_Result echo_recv(uint8_t* byte, bool* data_received)
{
    if (buffer_sz_ == 0) {
        usleep(10000);
        return F_OK;
    }
    *data_received = true;
    pthread_mutex_lock(&mutex);
    *byte = buffer_[0];
    memmove(buffer_, &buffer_[1], --buffer_sz_);
    pthread_mutex_unlock(&mutex);
    return F_OK;
}

FT_Result echo_send(const uint8_t* data, size_t sz)
{
    pthread_mutex_lock(&mutex);
    memcpy(&buffer_[buffer_sz_], data, sz);
    buffer_sz_ += sz;
    pthread_mutex_unlock(&mutex);
    return FT_OK;
}

void echo_finalize()
{
    pthread_mutex_destroy(&mutex);
}