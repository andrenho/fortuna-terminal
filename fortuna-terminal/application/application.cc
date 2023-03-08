#include "application.hh"
#include "control.hh"

#include <cstdlib>
#include <iostream>
#include <optional>

Application::Application(int argc, char* argv[])
    : options_(argc, argv),
      terminal_(options_.terminal_options)
{
    envs.emplace_back(options_);
    current_env = 0;

    // TODO - set debugging option

    gpio_.reset();

    for (Environment& env: envs)
        env.run_io_threads();

    control.emplace(ControlCommand::SetMode, options_.mode);
}

void Application::run()
{
    bool quit = false;
    try {

        while (!quit) {
            Environment& cur_ev = envs.at(current_env);

            frame_control_.start_frame();

            execute_control_commands();

            for (auto& env: envs)
                env.execute_step(frame_control_.avg_fps());

            gpio_.vsync();
            terminal_.do_events(cur_ev.events_interface(), &quit);
            terminal_.draw(cur_ev.scene());

            frame_control_.end_frame();
        }

    } catch (std::exception& e) {

        on_error(e, quit);
        if (!quit)
            run();
    }

    for (Environment& env: envs)
        env.finalize_threads();
}

void Application::on_error(std::exception const& e, bool& quit)
{
    std::cerr << "\e[1;31m" << e.what() << "\e0m\n";
    for (auto& env: envs)
        env.show_error(e);
    terminal_.draw(envs.at(current_env).scene());
    terminal_.wait_for_enter(&quit);
    if (!quit) {
        control.emplace(ControlCommand::Reset);
        run();
    }
}

void Application::execute_control_commands()
{
    Environment& cur_ev = envs.at(current_env);

    std::optional<Control> occ;
    while ((occ = control.pop_nonblock()).has_value()) {
        Control& cc = occ.value();
        switch (cc.command) {
            case ControlCommand::Reset:
                for (auto& env: envs)
                    env.reset();
                std::cout << "Terminal reset." << std::endl;
                break;
            case ControlCommand::ResetProtocol:
                cur_ev.reset();
                std::cout << "Scene reset." << std::endl;
                break;
            case ControlCommand::SetMode:
                cur_ev.set_mode(cc.mode);
                terminal_.resize_window(cur_ev.scene());
                std::cout << (cc.mode == Mode::Text ? "TextLayer" : "Graphics") << " mode set." << std::endl;
                break;
            case ControlCommand::SetMouseActive:
                terminal_.set_mouse_active(cc.active);
                break;
            case ControlCommand::SetMouseMoveReport:
                terminal_.set_mouse_register_move(cc.active);
                break;
            case ControlCommand::SetJoystickEmulation:
                terminal_.set_joystick_emulation(cc.active);
                break;
            case ControlCommand::ResetComputer:
                gpio_.reset();
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    std::optional<Application> application;
    try {
        application.emplace(argc, argv);
    } catch (std::exception& e) {
        std::cerr << "Error initializing terminal: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    application->run();

    return 0;
}

