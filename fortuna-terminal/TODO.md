- [x] Create stdin/stdout protocol
- [ ] Merge FortunaTerminal.h / remove SceneEvent queue?
  - [x] Remove fortuna-terminal.h
  - [x] Properly terminate
  - [x] Retest PTY
  - [x] Show error
  - [x] Add recommended flags
  - [x] Rename Protocol (?)
- [x] Update documentation (create reset, separate sprites from sprites image)
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
    - [x] Draw sprites
    - [x] Migrate to simple texture
  - [x] Tilemap
    - [x] Create images images
    - [x] Create maps
    - [x] Draw maps
    - [x] Multiple layers
  - [ ] Change palette
  - [ ] Enable/disable layer
  - [ ] Beep
  - [ ] Graphics collision support
  - [ ] Request version

- [x] Changes to scene
  - [x] constant identifying layers_ (enum)
  - [x] vector of layers_, functions to return text layer, sprites layer and images layers_ (n)
  - [x] should hold images (or, at least, create images)
  - [x] move terminal_size to text
  - [x] sort of unify SpriteLayer and TilemapLayer, use a single painter for both

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