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

inline void fifo_init(fifo_t *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

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

#endif
