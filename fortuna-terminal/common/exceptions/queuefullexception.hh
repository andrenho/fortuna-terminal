#ifndef QUEUEFULLEXCEPTION_HH_
#define QUEUEFULLEXCEPTION_HH_

#include <exception>
#include <string>

class QueueFullException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override { return "Queue full"; }

private:
    std::string message_;
};

#endif //FORTUNAEXCEPTION_HH_
