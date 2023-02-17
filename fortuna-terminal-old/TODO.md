Rewrite from C++
 - Communication
   - [x] TCP/IP
   - [x] PTY
   - [x] UART
 - Protocols
   - [x] ANSI (basic stuff)
 - Scene
   - [x] Text layer
 - UI
   - [x] Keyboard events / special keys
 - [x] Merge to master
   
Protocols
 - [x] ANSI (rewrite)
 - [x] Add reverse text
 - [ ] Fortuna Protocol (for text)
   - [ ] Print char
   - [ ] Text & formatting
   - [ ] 80 column mode
   - [ ] Mouse support
   - [ ] Joystick support
   - [ ] Queries
 - [ ] Create client library (TCP/IP first)

Additional signal wires
 - [ ] VSYNC
 - [ ] SUSPEND
 - [ ] RESET
   - [ ] On start, and on command
 - [ ] CMD (?)

Communication
 - UART 
    - [ ] Write UART/ANSI test code in microcontroller
    - [ ] UART in client library
 - SPI
   - [ ] SPI support in server
   - [ ] SPI in client library
 - I²C
    - [ ] I²C support in server
    - [ ] I²C in client library
 - TCP/IP client
   - [ ] Implement

Text
 - 80 columns
   - [ ] Implement
   - [ ] Option in initialization
   - [ ] Change from command

Emulator support
 - [ ] Write emulator support

Graphics
 - [ ] Mouse/joystick support
 - [ ] Graphics support
 - [ ] Collision detection

Audio
 - [ ] Audio support

Release
 - [ ] Create release version
 - [ ] Create fast-loading pi

Other
 - [ ] Bluetooth support
 - [ ] Additional communication (ex. debug over serial)
 - [ ] SSH communication