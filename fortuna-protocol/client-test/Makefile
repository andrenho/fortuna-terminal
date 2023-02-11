CPPFLAGS=-ggdb -O0 -Wall -Wextra -I..
OBJ = tcpip.o ../fortuna-protocol.o

all: tcpip

tcpip: ${OBJ}
	$(CC) -o $@ $^

clean:
ifeq ($(OS),Windows_NT)
	del /Q /F $(subst /,\,${OBJ}) tcpip.exe
else
	rm -f ${OBJ} tcpip
endif