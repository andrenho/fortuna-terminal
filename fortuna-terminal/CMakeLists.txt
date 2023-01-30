cmake_minimum_required(VERSION 3.24)
project(fortuna_terminal)

set(CMAKE_CXX_STANDARD 17)

add_executable(fortuna_terminal main.cc options.cc options.hh comm/communicationmodule.hh comm/echo.cc comm/echo.hh comm/communicationmodule.cc terminal/terminal.cc terminal/terminal.hh terminal/sdlterminal.cc terminal/sdlterminal.hh)
