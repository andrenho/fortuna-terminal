- Possible behaviours
  - Ignore error [will continue]
  - Handle error [will continue]
  - Report error on stdout (UI not yet available) [will not likely continue]
  - Report error to UI [will likely continue]
  - Abort [will not continue]

- Error types
  - application specific
  - libc
  - SDL
  - other library specific (communication)

- Can happen on main thread or not

- All functions that can fail will return int. Negative values are errors.
  - The return value should not be used to return actual values. That should be done via pointers in parameters.
  - There's a global error string that can be set with an error description.
  - A macro will help this process (will set the global error string and return -1).

- For treating errors, the following macros are provided:
  - Ignoring and handling errors don't need macros.
  - E_CHECK(..., str) will return on error, but won't do anything else
  - E_STDOUT(..., str) will report erros to stdout and then abort
  - E_UI(..., str) will report errors to the UI, and attempt to restart the terminal without changing the screen.
  - E_UI_ABORT(..., str) will report errors to the UI, wait for a keypress and then abort

- The following assertion are provided:
  - ASSERT(..., str) will print an error to the stdout and then abort. It's just E_STDOUT(0, str)
  
- Recoverable exceptions that show in E_UI will use longjmp or signals.
  - How to restart the threads?