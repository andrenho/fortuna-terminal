#ifndef ANSI_H_
#define ANSI_H_

#include <stdint.h>
#include <sys/types.h>

#include "../scene/scene.h"
#include "fortuna-protocol.h"
#include "buffer/buffer.h"

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene);
int ansi_terminal_event(FP_Command* command, Buffer* output_buffer);

#endif //ANSI_H_
