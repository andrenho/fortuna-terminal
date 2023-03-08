#include "application.hh"
#include "control.hh"

#include <cstdlib>
#include <iostream>
#include <optional>

Application::Application(int argc, char* argv[])
    : options(argc, argv)
{
    envs.emplace_back(options);
    current_env = 0;

    // TODO - set debugging option

    gpio_.reset();

    for (Environment& env: envs)
        env.run_io_threads();
}

void Application::run()
{
    bool quit = false;
    try {

        while (!quit) {
            frame_control_.start_frame();

            execute_control_commands();

            for (auto& env: envs)
                env.blink_cursor();
            envs.at(current_env).execute_step(frame_control_.avg_fps());

            gpio_.vsync();
            // TODO - terminal do events
            // TODO - terminal draw

            frame_control_.end_frame();
        }

    } catch (std::exception& e) {

        on_error(e, quit);
        if (!quit)
            run();
    }
}

void Application::on_error(std::exception const& e, bool& quit)
{
    std::cerr << "\e[1;31m" << e.what() << "\e0m\n";
    for (auto& env: envs)
        env.show_error(e);
    // TODO - terminal draw
    // TODO - terminal - wait for enter
    if (!quit) {
        control.emplace(ControlCommand::Reset);
        run();
    }
}

void Application::execute_control_commands()
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

