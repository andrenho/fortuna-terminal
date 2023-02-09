#ifndef BUFFER_H_
#define BUFFER_H_

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint8_t*        data;
    size_t          sz;
    size_t          max_sz;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
} Buffer;

void buffer_init(Buffer* buffer, size_t max_sz);
void buffer_finalize(Buffer* buffer);

int buffer_add_byte(Buffer* buffer, uint8_t byte);
int buffer_add_bytes(Buffer* buffer, uint8_t* bytes, size_t sz);
int buffer_add_str_nonull(Buffer* buffer, const char* str);
ssize_t buffer_move_data_to_array(Buffer* buffer, uint8_t* dest, size_t max_sz);

#endif //BUFFER_H_
