#ifndef COMM_H_
#define COMM_H_

#include "../options/options.h"
#include "buffer/buffer.h"

int    comm_init(size_t lines, size_t columns);
int    comm_run_input(Buffer* input_buffer);
int    comm_run_output(Buffer* output_buffer);
int    comm_notify_vsync();
int    comm_finalize();

#endif //COMM_H_
