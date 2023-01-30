#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <memory>
#include "../options.hh"

class CommunicationModule
{
protected:
    CommunicationModule() = default;

public:
    virtual void initialize() {}
    virtual void update() {}

    static std::unique_ptr<CommunicationModule> make_communication_module(Options const& options);
};

#endif //COMMUNICATIONMODULE_HH_
