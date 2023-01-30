OBJ=main.o debug.o options.o \
	scene/textlayer.o \
    protocol/protocol.o protocol/fortunaprotocol.o protocol/ansiprotocol.o \
	comm/comm_module.o comm/echo.o \
	terminal/terminal.o terminal/sdl/sdl_terminal.o terminal/sdl/sdl_painter.o
EXE=fortuna-terminal

CPPFLAGS=-Wall -Wextra -std=c++17 -I.

ifeq ($(OS),Windows_NT)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} -ISDL2-windows/include -O0 -ggdb
	LDFLAGS = -LSDL2-windows/lib -lSDL2main -lSDL2
else
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags` -O3
	LDFLAGS := ${CFLAGS} `sdl2-config --libs`
endif

all: ${EXE}

${EXE}: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE})
else
	rm -f ${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE}
endif