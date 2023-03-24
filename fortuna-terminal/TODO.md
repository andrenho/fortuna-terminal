Refactoring:
  - [x] Can frequent creation of thread become a thread with locks?
  - [x] Rename CommFullDuplex and CommXchg to single and full duplex

Defects:
 - [x] Garbage when more than one screenful

Electronics:
- [x] Build circuit
- [x] Write sample software for UART on the test circuit
- [x] Gpio

Improvements:
  - [x] Add FPS counter
  - [x] Print welcome information (welcome message, connection details, colors, version number)
  - [x] 32 colors
  - [x] Other text options (standout, etc)
  - [x] ACS_CHARS
  - [x] Review performance
    - [x] Show a marker on queue overload
  - [ ] Script language
     - [ ] Command compression
  - [ ] Change palette
  - [ ] Reduce CPU usage
  - [ ] Write client library
  - [ ] Text mode should be default, and graphics mode should be applied later
  - [ ] Fix TCP/IP
      - [ ] Add TCP/IP address to description

Refactor-4:
  - [x] Fixed 30 FPS
  - [x] Reverse is slow
  - [x] Refactor frame control (separate debugging info and frame wait)
  - [ ] Rewrite comms: https://www.linuxtoday.com/blog/blocking-and-non-blocking-i-0/
    - [x] Pipes
    - [x] PTY
    - [x] UART
    - [ ] I²C, SPI
    - [ ] TCP/IP

Alternative screens:
  - [ ] Terminal (shell)
  - [ ] Additional info

Emulator support
  - [ ] Emulator

Audio:
  - [ ] Design audio
  - [ ] Implement audio
  - [ ] Beep

New communication modes:
- [x] SPI
  - [x] Add delay between bytes option, join speed options
- [x] I²C
  - [x] Move Comm_IO to multiple bytes
