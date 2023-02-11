#ifndef LIBFORTUNA_H_
#define LIBFORTUNA_H_

#include <stdint.h>

#include "../fortuna-protocol.h"

struct FortunaTerminal;

void ft_init(struct FortunaTerminal* ft, void(* receive_data)(FP_Command* cmds, size_t sz), void* data);
void ft_send(struct FortunaTerminal* ft, FP_Command* cmds, size_t sz);
void ft_finalize(struct FortunaTerminal* ft);

#endif //LIBFORTUNA_H_
