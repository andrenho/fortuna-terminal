EXE := tests
OBJ := test.o ../ft-client.o

CFLAGS=-Wall -Wextra -ggdb -O0 -I..

ifeq ($(OS),Windows_NT)
	EXE := tests.exe
	CFLAGS += -I../win32/include
	LDFLAGS := -L../win32/lib -lws2_32
else
endif

LDFLAGS+=-lpng

all: ${EXE}

${EXE}: ${OBJ}
	gcc -o $@ ${OBJ} $(LDFLAGS)

test: ${EXE}
	./${EXE}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${EXE})
else
	rm -f ${OBJ} ${EXE}
endif