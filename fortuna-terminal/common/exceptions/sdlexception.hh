#ifndef SDLEXCEPTION_HH_
#define SDLEXCEPTION_HH_

#include <exception>
#include <string>

#include "lib/SDL2-windows/include/SDL2/SDL.h"

class SDLException : public std::exception {
public:
    explicit SDLException(std::string const& message) : message_(message + ": " + SDL_GetError()) {}
    [[nodiscard]] const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

#endif //SDLEXCEPTION_HH_
