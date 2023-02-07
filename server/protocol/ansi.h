#ifndef ANSI_H_
#define ANSI_H_

#include <stdint.h>
#include "../scene/scene.h"
#include "../../fortuna-protocol.h"

int  ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene);
void ansi_terminal_event(FP_Command* command, uint8_t* buffer, size_t* sz, size_t max_bufsz);

#endif //ANSI_H_