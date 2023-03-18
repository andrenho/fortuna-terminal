Refactoring:
  - [x] Can frequent creation of thread become a thread with locks?
  - [ ] Rename CommIO and CommXchg to single and full duplex

Electronics:
- [x] Build circuit
- [x] Write sample software for UART on the test circuit
- [ ] Gpio

Improvements:
  - [x] Add FPS counter
  - [ ] Command compression
  - [ ] 32 colors
  - [ ] Change palette
  - [ ] Activate command to start receiving VSYNC (?)
  - [ ] Other text options (standout, etc)
  - [ ] Print welcome information (welcome message, connection details, colors, version number)

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
