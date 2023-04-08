LIB := ftclient.so
OBJ := ftclient-lua.o ../ftclient.o ../ftclient-png.o ../ftclient-tcpip.o

CFLAGS=-Wall -Wextra -ggdb -O0 -I..

ifeq ($(OS),Windows_NT)
	LIB := ftclient.dll
	CFLAGS += -I../win32/include
	LDFLAGS := -L../win32/lib -lws2_32 -llua -lpng
else
	CFLAGS += `pkg-config --cflags lua5.4 libpng`
	LDFLAGS := `pkg-config --libs lua5.4 libpng`
endif

all: ${LIB}

${LIB}: ${OBJ}
	gcc -shared -o $@ ${OBJ} ${LDFLAGS}

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ} ${LIB})
else
	rm -f ${OBJ} ${LIB}
endif
