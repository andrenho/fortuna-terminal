#ifndef CONTROL_HH_
#define CONTROL_HH_

#include "common/syncqueue.hh"

enum class ControlCommand { Reset, ResetProtocol, SetTextMode, SetGraphicsMode };

extern SyncQueue<ControlCommand> control_commands;

#endif //CONTROL_HH_
