#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <fortuna-protocol.h>

#include "../options/options.h"
#include "scene/scene.h"
#include "buffer/buffer.h"

FT_Result protocol_init(Buffer* output_buffer, Scene* scene);
void      protocol_process_input(Buffer* input_buffer, Scene* scene);
void      protocol_terminal_event(FP_Message* message);
int       protocol_finalize();

#endif //PROTOCOL_H_
