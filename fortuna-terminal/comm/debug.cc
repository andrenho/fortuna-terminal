#include "debug.hh"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

void Debug::run_input_from_device_thread()
{
    while (running_) {
        std::string input;
        std::cin >> input;

        std::stringstream ss(input);
        std::vector<unsigned char> bytes;
        unsigned int temp;
        while (ss >> std::hex >> temp) {
            bytes.push_back(temp);
        }

        for (const auto& byte : bytes)
            protocol_.input_char(byte);
    }
}

void Debug::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue_.dequeue_block();
        std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(c) << ' ';
        std::flush(std::cout);
    }
}
