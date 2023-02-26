OBJ =   main.o options.o \
		comm/comm.o comm/fdcomm.o comm/echo.o comm/tcpip.o comm/pipes.o \
		protocol/protocol.o protocol/ansi.o protocol/extra.o \
        scene/scene.o \
		scene/layers/textlayer.o \
		scene/palette/palette.o \
        terminal/terminal.o terminal/texturemanager.o \
        terminal/painters/textpainter.o terminal/painters/spritepainter.o \
        lib/tmt/tmt.o

EXE = fortuna-terminal

CPPFLAGS=-I. -D_GNU_SOURCE -pthread
CXXFLAGS=-std=c++20

ifeq ($(OS),Windows_NT)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} @warnings.cfg -Ilib/SDL2-windows/include -O0 -ggdb
	LDFLAGS = -lws2_32 -Llib/SDL2-windows/lib -lssp -lmingw32 -lSDL2main -lSDL2 -lpthread
else
	OBJ := ${OBJ} comm/pty.o comm/uart.o
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags` -Wno-psabi -O3 -ggdb -DCOMM_UART -DCOMM_PTY
	LDFLAGS := ${CFLAGS} `sdl2-config --libs` -lpthread -lutil
endif

all: ${EXE}

lib/tmt/tmt.o: lib/tmt/tmt.c
	${CC} -c -o $@ $^ -O0 -ggdb

${EXE}: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE})
else
	rm -f ${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE}
endif
