EXE=tcpip-example
OBJ=tcpip.o ../ft-client.o

CFLAGS=-Wall -Wextra -ggdb -O0

ifeq ($(OS),Windows_NT)
	LDFLAGS := -lws2_32
else
endif

all: ${EXE}

${EXE}: ${OBJ}
	gcc -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${EXE})
else
	rm -f ${OBJ} ${EXE}
endif