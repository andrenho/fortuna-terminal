#ifndef FIFO_H_
#define FIFO_H_

#include <stdbool.h>
#include <stdint.h>

#define FIFO_SIZE 512

typedef struct {
    uint8_t buffer[FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} fifo_t;

void fifo_init(fifo_t *fifo);

inline bool fifo_push(fifo_t *fifo, uint8_t data)
{
    if ((fifo->head + 1) % FIFO_SIZE == fifo->tail) {
        return false; // full
    }
    fifo->buffer[fifo->head] = data;
    fifo->head = (fifo->head + 1) % FIFO_SIZE;
    return true;
}

inline bool fifo_pop(fifo_t *fifo, uint8_t *data)
{
    if (fifo->head == fifo->tail) {
        return false; // empty
    }
    *data = fifo->buffer[fifo->tail];
    fifo->tail = (fifo->tail + 1) % FIFO_SIZE;
    return true;
}

inline uint16_t fifo_size(fifo_t *fifo)
{
    return fifo->head - fifo->tail;
}

extern fifo_t in_fifo;
extern fifo_t out_fifo;

#endif
