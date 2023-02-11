#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../options/options.h"
#include "scene/scene.h"
#include "fortuna-protocol.h"
#include "buffer/buffer.h"

int  protocol_init(Buffer* output_buffer, Scene* scene);
void protocol_process_input(Buffer* input_buffer, Scene* scene);
void protocol_terminal_event(FP_Command* command);
int  protocol_finalize();

#endif //PROTOCOL_H_
