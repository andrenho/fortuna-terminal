#include "buffer.h"
#include "../error/error.h"

#include <stdlib.h>
#include <string.h>

#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

void buffer_init(Buffer* buffer, size_t max_sz)
{
    memset(buffer, 0, sizeof(Buffer));
    buffer->max_sz = max_sz;
    buffer->data = calloc(1, max_sz);
    pthread_mutex_init(&buffer->mutex, NULL);
    pthread_cond_init(&buffer->cond, NULL);
}

void buffer_finalize(Buffer* buffer)
{
    pthread_cond_destroy(&buffer->cond);
    pthread_mutex_destroy(&buffer->mutex);
    free(buffer->data);
}

int buffer_add_byte(Buffer* buffer, uint8_t byte)
{
    E_UI(FT_ERR_APP, "Buffer overflow");

    if (buffer->sz >= buffer->max_sz)
        E_UI(FT_ERR_APP, "Buffer overflow");

    pthread_mutex_lock(&buffer->mutex);
    buffer->data[buffer->sz++] = byte;
    pthread_cond_signal(&buffer->cond);
    pthread_mutex_unlock(&buffer->mutex);

    return 0;
}

int buffer_add_bytes(Buffer* buffer, uint8_t* bytes, size_t sz)
{
    size_t i = min(sz, buffer->max_sz - buffer->sz);

    if (i > 0) {
        pthread_mutex_lock(&buffer->mutex);
        memcpy(&buffer->data[buffer->sz], bytes, i);
        buffer->sz += i;
        pthread_cond_signal(&buffer->cond);
        pthread_mutex_unlock(&buffer->mutex);
    }

    return 0;
}

int buffer_add_str_nonull(Buffer* buffer, const char* str)
{
    unsigned long long n = strlen(str);
    if (n > 0)
        buffer_add_bytes(buffer, (uint8_t *) str, n);
    return 0;
}

ssize_t buffer_move_data_to_array(Buffer* buffer, uint8_t* dest, size_t max_sz)
{
    size_t sz = min(buffer->sz, max_sz);

    if (sz > 0) {
        pthread_mutex_lock(&buffer->mutex);
        memcpy(dest, buffer->data, sz);
        buffer->sz = 0;
        pthread_cond_signal(&buffer->cond);
        pthread_mutex_unlock(&buffer->mutex);
    }

    return sz;
}

void buffer_wait(Buffer* buffer)
{
    pthread_mutex_lock(&buffer->mutex);
    pthread_cond_wait(&buffer->cond, &buffer->mutex);
    pthread_mutex_unlock(&buffer->mutex);
}
