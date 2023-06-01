#include "echonumeric.hh"
#include "env/protocol/varint.hh"

#include <cstdio>
#include <iostream>

EchoNumeric::EchoNumeric()
{
    thread_.run_without_wait([this]() {
        int n;
        std::cin >> n;
        numbers_.push(n);
    });
}

std::vector<uint8_t> EchoNumeric::exchange(std::vector<uint8_t> data_to_send)
{
    for (auto byte: data_to_send)
        printf("%02X ", byte);
    fflush(stdout);

    std::vector<int> ints;
    numbers_.pop_all_into_noblock(ints);

    return to_varint(ints);
}

