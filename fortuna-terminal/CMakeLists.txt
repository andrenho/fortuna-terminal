cmake_minimum_required(VERSION 3.24)
project(fortuna_terminal)

set(CMAKE_CXX_STANDARD 17)

add_executable(fortuna_terminal main.cc options.cc options.hh comm/communicationmodule.hh comm/echo.cc comm/echo.hh comm/communicationmodule.cc terminal/terminal.cc terminal/terminal.hh terminal/sdlterminal.cc terminal/sdlterminal.hh debug.cc debug.hh)

if (WIN32)
    target_include_directories(fortuna_terminal PUBLIC ${CMAKE_SOURCE_DIR}/SDL2-windows/include)
    target_link_directories(fortuna_terminal PUBLIC ${CMAKE_SOURCE_DIR}/SDL2-windows/lib)
    target_link_libraries(fortuna_terminal mingw32 SDL2main SDL2)
endif (WIN32)

