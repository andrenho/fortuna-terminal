#ifndef COMM_H_
#define COMM_H_

#include "../options/options.h"

int    comm_init(Options* options);
int    comm_run();
size_t comm_unload_input_queue(uint8_t* dest, size_t max_sz);
int    comm_add_to_output_queue(uint8_t* data, uint8_t sz);
int    comm_notify_vsync();
int    comm_finalize();

#endif //COMM_H_
