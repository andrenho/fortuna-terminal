#
# Makefile configuration
#

RELEASE ?= 0
FAST ?= 0

WINDOWS := 0
RASPBERRY := 0
INCLUDE_UART_PTY := 0
INCLUDE_SPI_I2C := 0
COLOR_TERMINAL := 0

# autodetect configuration
ifeq ($(OS),Windows_NT)
	WINDOWS := 1
else
	COLOR_TERMINAL := 1
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

OBJ =   main.o \
		application/application.o application/options.o application/control.o application/framecontrol.o \
		env/environment.o env/threadrunner.o \
        env/comm/comm.o \
        env/comm/io/comm_io.o env/comm/io/echo.o \
        env/comm/io/fd/fdcomm.o env/comm/io/fd/pipes.o env/comm/io/fd/tcpip.o \
		env/protocol/protocol.o env/protocol/ansi.o env/protocol/extra.o env/protocol/events.o \
 		scene/scene.o scene/palette/palette.o scene/layers/images.o \
 		scene/layers/spritelayer.o scene/layers/textlayer.o scene/layers/tilemaplayer.o \
 		terminal/terminal.o terminal/texturemanager.o \
 		terminal/painters/textpainter.o terminal/painters/graphicspainter.o \
		lib/tmt/tmt.o

DEPS := $(OBJ:.o=.d)

EXE = fortuna-terminal

CPPFLAGS=-I. -D_GNU_SOURCE -pthread -DCOLOR_TERMINAL=${COLOR_TERMINAL}
CXXFLAGS=-std=c++20
LDFLAGS=-pthread

GCH_HEADER=precompile.hh

ifeq ($(WINDOWS),1)
	EXE := ${EXE}.exe
	CPPFLAGS := ${CPPFLAGS} -Ilib/SDL2-windows/include
	LDFLAGS := ${LDFLAGS} -lws2_32 -Llib/SDL2-windows/lib -lssp -lmingw32 -lSDL2main -lSDL2
else
	CPPFLAGS := ${CPPFLAGS} `sdl2-config --cflags`
	LDFLAGS := ${LDFLAGS} `sdl2-config --libs` -lpthread -lutil
	GCH_HEADER_BIN=${GCH_HEADER}.gch
	INCLUDE_HEADER=-include precompile.hh
endif

ifeq ($(RASPBERRY),1)
	CPPFLAGS := ${CPPFLAGS} -Wno-psabi
endif

ifeq ($(INCLUDE_UART_PTY),1)
	OBJ := ${OBJ} env/comm/io/fd/pty.o env/comm/io/fd/uart.o
	CPPFLAGS := ${CPPFLAGS} -DCOMM_UART -DCOMM_PTY
endif

ifeq ($(FAST),0)
	CPPFLAGS := ${CPPFLAGS} @warnings.cfg
endif

ifeq ($(RELEASE),1)
	CPPFLAGS := ${CPPFLAGS} -O3 -flto
	LDFLAGS := ${LDFLAGS} -O3 -flto
else
	CPPFLAGS := ${CPPFLAGS} -O0 -ggdb
endif

#
# Rules
#

all: ${EXE}

lib/tmt/tmt.o: lib/tmt/tmt.c
	${CC} -c -o $@ $^ -O0 -ggdb

${GCH_HEADER}.gch: ${GCH_HEADER}
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c $^ -o $@

%.o: %.cc ${GCH_HEADER_BIN}
	${CXX} ${CXXFLAGS} ${CPPFLAGS} ${INCLUDE_HEADER} -MMD -MF $(patsubst %.o,%.d,$@) -c -o $@ $<

${EXE}: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

-include ${DEPS}

.PHONY: clean
clean:
ifeq ($(WINDOWS),1)
	del /Q /F $(subst /,\,${OBJ} ${EXE} ${GCH_HEADER_BIN} ${DEPS})
else
	rm -f ${OBJ} ${EXE} ${GCH_HEADER_BIN} ${DEPS}
endif

