#ifndef COMM_H_
#define COMM_H_

#include "../options/options.h"

int comm_init(Options* options);
int comm_run();
int comm_notify_vsync();
int comm_stop();

#endif //COMM_H_
