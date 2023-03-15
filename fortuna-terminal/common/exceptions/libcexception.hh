#ifndef LIBCEXCEPTION_HH_
#define LIBCEXCEPTION_HH_

#include <exception>
#include <string>
#include <cstring>

class LibcException : public std::exception {
public:
    explicit LibcException(std::string const& message) : message_(message + ": " + strerror(errno)) {}
    [[nodiscard]] const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

#endif //LIBCEXCEPTION_HH_
