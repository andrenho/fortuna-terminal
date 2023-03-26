#ifndef CONTROL_HH_
#define CONTROL_HH_

#include "common/syncqueue.hh"
#include "common/enums/mode.hh"

enum class ControlCommand {
    Reset, ResetProtocol, ResetComputer, SetMode, SetMouseActive, SetMouseMoveReport, SetJoystickEmulation, EnableVSYNC,
};

struct Control {

    ControlCommand command;

    union {
        Mode mode;
        bool active;
    };

    explicit constexpr Control(ControlCommand command) : command(command), active(false) {}
             constexpr Control(ControlCommand command, Mode mode) : command(command), mode(mode) {}
             constexpr Control(ControlCommand command, bool active) : command(command), active(active) {}
};

extern SyncQueue<Control> control_queue;

extern template class SyncQueue<Control>;

#endif //CONTROL_HH_
