OBJ =   main.o \
        terminal/terminal.o \
        terminal/scene/scene.o \
        terminal/painters/textpainter.o

EXE = fortuna-terminal

CPPFLAGS=-Wall -Wextra -I. -I ../fortuna-protocol \
		 -Wwrite-strings -Wshadow \
		 -D_GNU_SOURCE -pthread
CXXFLAGS=-std=c++20

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

#protocol/tmt.o: protocol/tmt.c
# 	${CC} -c -o $@ $^ -O0 -ggdb

${EXE}: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE})
else
	rm -f ${OBJ} ${STDIN_OBJ} ${SERIAL_OBJ} ${EXE}
endif
