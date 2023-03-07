#include "application.hh"

#include <cstdlib>
#include <iostream>
#include <optional>

Application::Application(int argc, char* argv[])
    : options(argc, argv)
{
    envs.emplace_back(options);
}

void Application::run()
{

}

int main(int argc, char* argv[])
{
    std::optional<Application> application;
    try {
        application = Application(argc, argv);
    } catch (std::exception& e) {
        std::cerr << "Error initializing terminal: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    application->run();
}

