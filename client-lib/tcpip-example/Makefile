EXE=tcpip-example
OBJ=tcpip.o ../ft-client.o

CFLAGS=-Wall -Wextra -ggdb -O0 -DFT_PNG_SUPPORT

ifeq ($(OS),Windows_NT)
	CFLAGS += -I../win32/include
	LDFLAGS := -L../win32/lib -lws2_32
else
endif

LDFLAGS+=-lpng

all: ${EXE}

${EXE}: ${OBJ}
	gcc -dumpmachine
	gcc -o $@ ${OBJ} $(LDFLAGS)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${EXE})
else
	rm -f ${OBJ} ${EXE}
endif