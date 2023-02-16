OBJ=main.o \
	comm/comm.o comm/echo.o comm/tcpip.o comm/debug.o \
	error/error.o \
	gpio/gpio.o \
	options/options.o \
	buffer/buffer.o \
	protocol/protocol.o protocol/ansi.o protocol/tmt.o protocol/fortuna.o \
	../fortuna-protocol/fortuna-protocol.o \
	scene/scene.o scene/text.o scene/palette.o \
	ui/ui.o ui/painter.o
EXE=fortuna-terminal

CPPFLAGS=-Wall -Wextra -I. -I ../fortuna-protocol \
		 -fstack-protector -fstack-protector-strong -Wformat=2 -Wpointer-arith -Wunreachable-code -Wwrite-strings\
		 -D_GNU_SOURCE -pthread
CFLAGS=-std=c11

ifeq ($(OS),Windows_NT)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} -Ilib/SDL2-windows/include -O0 -ggdb
	LDFLAGS = -lws2_32 -Llib/SDL2-windows/lib -lssp -lmingw32 -lSDL2main -lSDL2 -lpthread
else
	OBJ := ${OBJ} comm/pty.o comm/uart.o
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags` -O0 -ggdb -DCOMM_UART -DCOMM_PTY
	LDFLAGS := ${CFLAGS} `sdl2-config --libs` -lpthread -lutil
endif

all: ${EXE}

protocol/tmt.o: protocol/tmt.c
	${CC} -c -o $@ $^ -O0 -ggdb

${EXE}: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE})
else
	rm -f ${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE}
endif
