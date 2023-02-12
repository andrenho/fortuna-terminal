CPPFLAGS=-ggdb -O0 -Wall -Wextra -I..
OBJ =

all: tcpip test

tcpip: tcpip.o ../fortuna-protocol.o
	$(CC) -o $@ $^

test: test.o ../fortuna-protocol.o
	$(CC) -o $@ -ggdb -O0 $^

clean:
ifeq ($(OS),Windows_NT)
	del /Q /F *.o ..\fortuna-protocol.o tcpip.exe
else
	rm -f ${OBJ} tcpip
endif