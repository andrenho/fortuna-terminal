#include "communicationmodule.hh"

#include <iostream>

#include "echo.hh"

std::unique_ptr<CommunicationModule> CommunicationModule::make_communication_module(Options const& options)
{
    switch (options.communication_mode) {
        case CommunicationMode::Echo:
            return std::make_unique<Echo>();
        default:
            std::cerr << "Unsupported communication module.\n";
            exit(EXIT_FAILURE);
    }
}
