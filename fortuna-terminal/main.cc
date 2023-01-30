#include <iostream>

#include <getopt.h>
#include <memory>

#include "comm/communicationmodule.hh"
#include "options.hh"
#include "terminal/terminal.hh"
#include "debug.hh"

int main(int argc, char* argv[])
{
    const Options options(argc, argv);
    debug_mode = options.debug_mode;

    auto communication_module = CommunicationModule::make_communication_module(options);
    auto terminal = Terminal::make_terminal(options, *communication_module);

    terminal->initialize();
    communication_module->initialize();
    while(terminal->running()) {
        // TODO - timing
        communication_module->update();
        terminal->update();
    }
}
