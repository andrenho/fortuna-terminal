#ifndef ECHO_H_
#define ECHO_H_

#include <stdint.h>
#include <stddef.h>

int echo_init();
int echo_recv(uint8_t* byte);
int echo_send(const uint8_t* data, size_t sz);
int echo_finalize();

#endif //ECHO_H_
