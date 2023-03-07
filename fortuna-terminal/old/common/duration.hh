#ifndef DURATION_HH_
#define DURATION_HH_

#include <chrono>

using Duration = decltype(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now());

#endif //DURATION_HH_
