#include "comm.hh"
#include "io/echo.hh"
#include "application/options.hh"
#include "common/exceptions/fortunaexception.hh"
#include "application/options.hh"

std::unique_ptr<CommunicationModule> CommunicationModule::create(Options const &options)
{
    switch (options.comm_type) {
        case CommType::NotChosen:
            throw FortunaException("Invalid communication mode");
        case CommType::Echo:
            return std::make_unique<Echo>();
    }

    // TODO - add other communcation modules

    return nullptr;
}
