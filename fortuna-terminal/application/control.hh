#ifndef CONTROL_HH_
#define CONTROL_HH_

#include "common/syncqueue.hh"
#include "common/mode.hh"

enum class ControlCommand { Reset, ResetProtocol, ResetComputer, SetMode, SetMouseActive, SetMouseMoveReport, SetJoystickEmulation };

struct Control {
    ControlCommand command;
    union {
        Mode mode;
        bool active;
    };

    explicit Control(ControlCommand command) : command(command) {}
    Control(ControlCommand command, Mode mode) : command(command), mode(mode) {}
    Control(ControlCommand command, bool active) : command(command), active(active) {}
};

extern SyncQueue<Control> control;

extern template class SyncQueue<Control>;

#endif //CONTROL_HH_
