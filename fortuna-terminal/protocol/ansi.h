#ifndef ANSI_H_
#define ANSI_H_

#include <fortuna-protocol.h>

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "../scene/scene.h"
#include "buffer/buffer.h"

int     ansi_init(Scene* scene);
ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene);
int     ansi_terminal_event(FP_Message* message, Buffer* output_buffer);
int     ansi_finalize();

#endif //ANSI_H_
