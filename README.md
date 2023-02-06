# Fortuna Terminal

![image](https://user-images.githubusercontent.com/84652/215353776-29afeb20-d336-4596-835f-49292c930799.png)

Fortuna Terminal is a terminal for the [Fortuna computer series](https://fortuna-computers.github.io/). As a terminal, it provides the following functionality:

 - A 40 or 80-column terminal
 - A graphic mode for games that offers:
   - A 256x256 pixel screen (that can be halved)
   - Two moveable background layers + one foreground fixed layer (for UI)
   - Up to 64 simultaneous sprites in 4 different layers
   - A 32 color palette, programmable per layer
   - 41x28 matrix of text
 - Audio via HDMI - 3 programmable wave audio + 1 PGM audio channel
 - Keyboard input
 - Mouse input
 - Joystick input
 
 The terminal can be connected to a computer, microprocessor or microcontroller through the following means:
 
 - UART - speeds from 300 bps up to 1Mbps
 - I²C - with the terminal operating as master, and requesting updates at regular intervals
 - SPI - with the terminal operating as master, and requesting updates at regular intervals
 - TCP/IP - also allowing connection via Wi-Fi on the Pi
 - Bluetooth (?)
 - emcc - for generating WebAssembly code and interacting with a javascript emulator
 - pty - connect a terminal to the underlying Linux of the terminal itself (the Raspberry Pi)
 - The following additional wires provide further functionality:
   - an VSYNC line indicates when a new frame has finished drawing
   - on RESET line allows the terminal to reset the computer
   - a SUSPEND line stops communication for some time (allowing the SPI lines to be used for something else, for example)

Communication can happen over the following protocols:

 - Fortuna: uses the [Fortuna Communication Protocol](https://github.com/andrenho/fortuna-terminal/wiki/Fortuna-Protocol). It provides error detection, flow control and retransmission in case of errors. With this protocol, all functionality in Fortuna Terminal is supported.
 - Ansi: support to the [Linux Ansi terminal protocol](https://espterm.github.io/docs/VT100%20escape%20codes.html), used by default by many Unix distributions. In this case, only text mode is supported.
 - Fortuna+Ansi: same as Ansi, but an additional wire allows special commands (such as graphics) to be sent via the same line.
 
 Techincal information:
 
See the wiki for [more detailed technical information](https://github.com/andrenho/fortuna-terminal/wiki)!
