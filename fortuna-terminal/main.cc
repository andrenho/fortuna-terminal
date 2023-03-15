#include <cstdio>
#include <optional>

#include "application/application.hh"

int main(int argc, char* argv[])
{
    std::optional<Application> application;
    try {
        application.emplace(argc, argv);
    } catch (std::exception& e) {
        fprintf(stderr, "Error initializing Fortuna Terminal: %s\n", e.what());
        exit(EXIT_FAILURE);
    }

    application->run();

    return 0;
}

