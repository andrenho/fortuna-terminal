#ifndef COMM_H_
#define COMM_H_

#include "../options/options.h"
#include "buffer/buffer.h"

FT_Result comm_init(size_t lines, size_t columns);
void      comm_run_input(Buffer* input_buffer);
void      comm_run_output(Buffer* output_buffer);
void      comm_notify_vsync();
void      comm_finalize();

#endif //COMM_H_
