#include "fifo.h"

fifo_t in_fifo;
fifo_t out_fifo;

void fifo_init(fifo_t *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}
