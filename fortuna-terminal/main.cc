#include <optional>

#include "application/application.hh"

#include <iostream>

int main(int argc, char* argv[])
{
    std::optional<Application> application;
    try {
        application.emplace(argc, argv);
    } catch (std::exception& e) {
        std::cerr << "Error initializing Fortuna Terminal: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    application->run();

    return 0;
}

