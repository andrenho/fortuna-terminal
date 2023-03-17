- [x] Create stdin/stdout protocol
- [ ] Merge FortunaTerminal.h / remove SceneEvent queue?
  - [x] Remove fortuna-terminal.h
  - [x] Properly terminate
  - [x] Retest PTY
  - [x] Show error
  - [x] Add recommended flags
  - [x] Rename Protocol (?)
- [x] Update documentation (create reset_ansi_protocol, separate sprites from sprites pixels)
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
  - [x] Create tool for generating image_indexes
  - [x] Sprites
    - [x] Create image_indexes
    - [x] Draw sprites
    - [x] Migrate to simple texture
  - [x] Tilemap
    - [x] Create image_indexes
    - [x] Create maps
    - [x] Draw maps
    - [x] Multiple layers
  - [x] Change palette
  - [x] Enable/disable layer
  - [x] Graphics collision support
  - [x] Request version
  - [x] Improve Makefile

- [x] Changes to scene
  - [x] constant identifying layers_ (enum)
  - [x] vector of layers_, functions to return text layer, sprites layer and image_indexes layers_ (n)
  - [x] should hold image_indexes (or, at least, create image_indexes)
  - [x] move terminal_size to text
  - [x] sort of unify SpriteLayer and TilemapLayer, use a single painter for both

Refactoring 3:
 - [x] Create runner
 - [x] Not working
     - [x] Cursor
     - [x] Graphics
     - [x] FPS
     - [x] Exceptions are not showing
     - [ ] Why 'pacman' is printing zeroes?
 - [x] Rearrange Protocol class
 - [x] I/O debugging
 - [x] Reset
 - [x] Scene id
 - [x] Refactor SDL RAII
 - [x] General overview + make compilation faster (?)
 - [x] Don't use pending_images, control that from graphics side
 - [x] Check reset for terminal

- For each class
  - [x] external templates
  - [X] precompiled headers
- For the project
  - [x] dependencies
  - [ ] ccache
  - [ ] compile remotely / cross-compile
  - [ ] ram disk
  - [x] LTO

Electronics:
  - [x] Build circuit
  - [ ] Write sample software for UART on the test circuit
  - [ ] Gpio

Improvements:
  - [x] Add FPS counter
  - [ ] Command compression
  - [ ] 32 colors
  - [ ] Change palette
  - [ ] Activate command to start receiving VSYNC (?)
  - [ ] Other text options (standout, etc)

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
- [ ] SPI
  - Wire in pin 16 should go to pin 26, then use /dev/spidev0.1
- [ ] I²C

