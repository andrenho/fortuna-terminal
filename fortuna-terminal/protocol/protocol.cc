#include "protocol.hh"

#include <csignal>

#include <algorithm>
#include <iostream>
#include <thread>
#include <iomanip>

#include "exceptions/fortunaexception.hh"

using namespace std::chrono_literals;
using namespace std::string_literals;

Protocol::Protocol(Mode mode, std::unique_ptr<CommunicationModule> comm, GPIO& gpio)
        : comm_(std::move(comm)),
          scene_(mode),
          ansi_(mode, scene_),
          extra_(mode, scene_, gpio),
          mode_(mode)
{
}

void Protocol::run()
{
    read_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            auto byte = comm_->read_blocking();
            if (byte.has_value()) {
                input_queue_->push(byte.value());
                debug_byte(true, byte.value());
            }
        }
    });

    output_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            std::vector<uint8_t> bytes_to_output;
            output_queue_->pop_all_into(bytes_to_output);
            comm_->write(bytes_to_output);
            if (debug_comm_)
                std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(false, byte); });
        }
    });
}

void Protocol::finalize_threads()
{
    threads_active_ = false;
    if (comm_->release_locks()) {
        read_thread_->join();
    } else {
        pthread_kill(read_thread_->native_handle(), 9);
        read_thread_->detach();
    }
    input_queue_->push({});  // release the lock
    output_queue_->push({});  // release the lock
    output_thread_->join();
}

void Protocol::event_text_input(std::string const &text)
{
    std::vector<uint8_t> v(text.begin(), text.end());
    output_queue_->push_all(v);
}

void Protocol::event_key(uint8_t key, bool is_down, KeyMod mod)
{
    if (is_down) {
        if (mod.control) {
            if (key >= 'a' && key <= 'z')
                output_queue_->push(key - 96);
        }
    }
}

void Protocol::event_key(SpecialKey key, bool is_down, KeyMod mod)
{
    if (is_down) {
        auto ostr = ansi_.translate_special_key(key, mod);
        if (ostr.has_value()) {
            std::vector<uint8_t> v(ostr.value().begin(), ostr.value().end());
            output_queue_->push_all(v);
        }
    }
}

void Protocol::event_mouse_button(int button, int x, int y, bool is_down)
{
    std::string s = "\e#"s + std::to_string(button) + ";" + std::to_string(x) + ";" + std::to_string(y) +
            ";" + (is_down ? "B" : "R");
    output_queue_->push_all(s);
}

void Protocol::event_mouse_move(int button, int x, int y)
{
    output_queue_->push_all("\e#"s + std::to_string(button) + ";" + std::to_string(x) + ";" + std::to_string(y) + "M");
}

void Protocol::event_joystick(size_t joystick_number, size_t button, bool is_down)
{
    output_queue_->push_all("\e#s"s + std::to_string(joystick_number) + ";" + std::to_string(button) + (is_down ? 'J' : 'K' ));
}

void Protocol::debug_byte(bool is_input, uint8_t byte)
{
    if (debug_comm_) {
        if (is_input)
            std::cout << "\e[1;33m";
        else
            std::cout << "\e[1;34m";
        if (byte >= 32 && byte < 127)
            std::cout << byte;
        else if (byte == '\e')
            std::cout << "[ESC]";
        else
            std::cout << "[" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int) byte << "]";
        if (byte == 10)
            std::cout << "\n";
        std::cout << "\e[0m" << std::flush;
    }
}

void Protocol::show_error(std::exception const &e)
{
    std::string message = "\e[0;31m"s + e.what() + "\r-- Press ENTER to continue or Ctrl+F12 to quit --\r\e[0m";
    std::vector<uint8_t> v(message.begin(), message.end());
    output_queue_->push_all(v);
    std::this_thread::sleep_for(10ms);
}

void Protocol::reset()
{
    input_queue_->clear();
    output_queue_->clear();
    ansi_.reset();
    scene_.reset();
}

void Protocol::execute_inputs()
{
    static std::string received_bytes;
    static bool extra_active = false;

    input_queue_->optionally_pop_all_into(received_bytes);

    bool last_is_esc = received_bytes.ends_with('\e');
    if (last_is_esc)
        received_bytes = received_bytes.substr(0, received_bytes.size() - 1);

    if (!received_bytes.empty()) {

        size_t current_pos = 0;
        while (true) {
            if (!extra_active) {
                // we're not in extra escape sequence, so let's look for a "ESC *" that starts that sequence
                // if we don't find it, we can send the whole thing to ANSI
                size_t next_esc_star = received_bytes.find("\e*", current_pos);
                if (next_esc_star == std::string::npos) {
                    ansi_.send_bytes(received_bytes.substr(current_pos));
                    break;
                } else {
                    extra_active = true;
                    ansi_.send_bytes(received_bytes.substr(current_pos, next_esc_star - current_pos));
                    current_pos = next_esc_star;
                }
            } else {
                auto it_next_alpha = std::find_if(received_bytes.begin() + current_pos, received_bytes.end(), [](char c) { return std::isalpha(c); });
                if (it_next_alpha == received_bytes.end()) {
                    extra_.send_bytes(received_bytes.substr(current_pos));
                    break;
                } else {
                    size_t offset = it_next_alpha - received_bytes.begin() + 1;
                    extra_active = false;
                    extra_.send_bytes(received_bytes.substr(current_pos, offset - current_pos));
                    current_pos = offset;
                }
            }
        }

        received_bytes.clear();
    }
    if (last_is_esc)
        received_bytes.push_back('\e');

}

void Protocol::set_mode(Mode mode)
{
    mode_ = mode;
    scene_.set_mode(mode);
    ansi_.set_mode(mode);
    extra_.set_mode(mode);
}
