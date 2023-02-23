#include "protocol.hh"

#include <csignal>

#include <iostream>
#include <thread>
#include <iomanip>

#include "exceptions/fortunaexception.hh"

using namespace std::chrono_literals;
using namespace std::string_literals;

Protocol::Protocol(std::unique_ptr<CommunicationModule> comm, GPIO& gpio)
        : comm_(std::move(comm)),
          ansi_(scene_),
          extra_(scene_, gpio)
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

    input_thread_ = std::make_unique<std::thread>([this]() {
        bool previous_was_esc = false;

        while (threads_active_) {
            std::vector<uint8_t> received_bytes;
            input_queue_->pop_all_into(received_bytes);
            for (uint8_t c : received_bytes) {
                if (c == '\e') {
                    previous_was_esc = true;
                } else {
                    if (previous_was_esc) {
                        if (c == '*')
                            extra_.send_bytes({ '\e', c });
                        else
                            ansi_.send_bytes({ '\e', c });
                        previous_was_esc = false;
                    } else if (extra_.escape_sequence_active()) {
                        extra_.send_bytes({ '\e', c });
                    } else {
                        ansi_.send_bytes({ c });
                    }
                }
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
    input_thread_->join();
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

