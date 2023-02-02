OBJ=main.o debugmode.o options.o \
	scene/scene.o scene/textlayer.o \
    protocol/protocol.o protocol/ansiprotocol.o protocol/fortunaprotocol.o \
	comm/comm_module.o comm/echo.o comm/tcpip.o comm/debug.o \
	terminal/terminal.o terminal/sdl/sdl_terminal.o terminal/sdl/sdl_painter.o
EXE=fortuna-terminal

CPPFLAGS=-Wall -Wextra -std=c++17 -I.

ifeq ($(OS),Windows_NT)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} -Ilib/SDL2-windows/include -O0 -ggdb
	LDFLAGS = -lws2_32 -Llib/SDL2-windows/lib -lSDL2main -lSDL2
else
	OBJ := ${OBJ} comm/uart.o comm/pty.o
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags` -O3 -DCOMM_UART -DCOMM_PTY
	LDFLAGS := ${CFLAGS} `sdl2-config --libs` -lpthread
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