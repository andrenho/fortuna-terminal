#ifndef TIME_HH_
#define TIME_HH_

#include <chrono>

using namespace std::chrono_literals;

using Time = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = decltype(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now());

#endif //TIME_HH_
