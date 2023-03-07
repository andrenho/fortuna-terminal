#
# Makefile configuration
#

RELEASE ?= 0
FAST ?= 0

WINDOWS := 0
RASPBERRY := 0
INCLUDE_UART_PTY := 0
INCLUDE_SPI_I2C := 0

# autodetect configuration
ifeq ($(OS),Windows_NT)
	WINDOWS := 1
else
	INCLUDE_UART_PTY := 1
	ifeq ($(shell uname -m),armv7l)
		ifeq ($(shell cat /proc/cpuinfo | grep -q BCM; echo $$?),0)
			RASPBERRY := 1
			INCLUDE_SPI_I2C := 1
		endif
	endif
endif

#
# configure objects and compilation options
#

OBJ =   application/application.o \
		lib/tmt/tmt.o

EXE = fortuna-terminal

CPPFLAGS=-I. -D_GNU_SOURCE -pthread
CXXFLAGS=-std=c++20
LDFLAGS=-pthread

ifeq ($(WINDOWS),1)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} -Ilib/SDL2-windows/include
	LDFLAGS := ${LDFLAGS} -lws2_32 -Llib/SDL2-windows/lib -lssp -lmingw32 -lSDL2main -lSDL2
else
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags`
	LDFLAGS := ${LDFLAGS} `sdl2-config --libs` -lpthread -lutil
endif

ifeq ($(RASPBERRY),1)
	CPPFLAGS := ${CPPFLAGS} -Wno-psabi
endif

ifeq ($(INCLUDE_UART_PTY),1)
	OBJ := ${OBJ} comm/pty.o comm/uart.o
	CPPFLAGS := ${CPPFLAGS} -DCOMM_UART -DCOMM_PTY
endif

ifeq ($(FAST),0)
	CPPFLAGS := ${CPPFLAGS} @warnings.cfg
endif

ifeq ($(RELEASE),1)
	CPPFLAGS := ${CPPFLAGS} -O3
else
	CPPFLAGS := ${CPPFLAGS} -O0 -ggdb
endif

#
# Rules
#

all: ${EXE}

lib/tmt/tmt.o: lib/tmt/tmt.c
	${CC} -c -o $@ $^ -O0 -ggdb

${EXE}: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(WINDOWS),1)
	del /Q /F $(subst /,\,${OBJ} ${EXE})
else
	rm -f ${OBJ} ${EXE}
endif
