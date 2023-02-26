- [x] Create stdin/stdout protocol
- [ ] Merge FortunaTerminal.h / remove SceneEvent queue?
  - [x] Remove fortuna-terminal.h
  - [x] Properly terminate
  - [x] Retest PTY
  - [x] Show error
  - [x] Add recommended flags
  - [x] Rename Protocol (?)
- [x] Update documentation (create reset, separate sprite from sprite image)
- [ ] Create test circuit

New protocol:
  - [x] Activate graphics mode
    - [x] Adjust classes
    - [x] Create switch to start in graphic mode
    - [x] Implement swap via commands
  - [x] Mouse support
  - [x] Joystick support
    - [x] Enable/disable joystick
    - [x] Joystick buttons
    - [x] Joystick directional
    - [x] Joystick keyboard emulation
  - [x] Set background color
  - [x] Create tool for generating images
  - [x] Sprites
    - [x] Create images
    - [x] Draw sprite
    - [ ] Migrate to simple texture
    - [ ] Change palette
  - [ ] Tilemap
    - [ ] Create tilemap images
    - [ ] Create maps
    - [ ] Draw maps
    - [ ] Change palette
    - [ ] Multiple layers
  - [ ] Enable/disable layer
  - [ ] Graphics collision support
  - [ ] Request version

Alternative screens:
  - [ ] Terminal (shell)
  - [ ] Additional info

Emulator support
  - [ ] Emulator

Audio:
  - [ ] Design audio
  - [ ] Implement audio

New communication modes:
  - [ ] SPI
  - [ ] I²C