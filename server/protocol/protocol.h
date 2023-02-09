#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../options/options.h"
#include "scene/scene.h"
#include "fortuna-protocol.h"
#include "buffer/buffer.h"

int  protocol_init(Options* options, Buffer* output_buffer);
void protocol_process_input_queue(Scene* scene);
void protocol_terminal_event(FP_Command* command);

#endif //PROTOCOL_H_
