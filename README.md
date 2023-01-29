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
 - UART I/O - provide functionality to extend I/O to serial
 
 The terminal can be connected to a microprocessor or microcontroller through the following means:
 
 - UART - speeds from 300 bps up to 1Mbps
 - I²C - with the terminal operating as a slave
 - SPI - with the terminal operating as master, and requesting updates at regular intervals
