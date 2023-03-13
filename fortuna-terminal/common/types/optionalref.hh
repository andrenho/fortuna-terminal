#ifndef OPTIONALREF_HH_
#define OPTIONALREF_HH_

#include <optional>
#include <functional>

template <typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;

#endif //OPTIONALREF_HH_
