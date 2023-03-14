#ifndef UNIQUE_PTR_HH_
#define UNIQUE_PTR_HH_

#include <functional>
#include <memory>

template <typename T>
using UniquePtrWithDeleter = std::unique_ptr<T, std::function<void(T*)>>;

#endif //UNIQUE_PTR_HH_
