# Fortuna Terminal

![image](https://user-images.githubusercontent.com/84652/215353776-29afeb20-d336-4596-835f-49292c930799.png)

### Functionalities

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
 
 ### Communication
 
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

### Protocols

The communication protocol is an extension of the terminal ANSI protocol, allowing for text, graphics and audio.

- Fortuna Terminal uses [libtmt](https://github.com/deadpixi/libtmt) for supporting the ANSI protocol for text. Here's a [list of escape codes supported](https://github.com/deadpixi/libtmt#supported-input-and-escape-sequences).
- Fortuna Terminal protocol extension supports graphics and audio, also also reports mouse and joystick events back to the application. 
 
### Additional techincal information
 
See the wiki for [more detailed technical information](https://github.com/andrenho/fortuna-terminal/wiki)!
