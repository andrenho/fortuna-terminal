#ifndef FORTUNAEXCEPTION_HH_
#define FORTUNAEXCEPTION_HH_

#include <exception>
#include <string>

class FortunaException : public std::exception {
public:
    explicit FortunaException(std::string const& message) : message_(message + ".") {}
    [[nodiscard]] const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

#endif //FORTUNAEXCEPTION_HH_
