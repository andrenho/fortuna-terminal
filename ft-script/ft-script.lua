#!/bin/sh lua

function reset_terminal()
    print("\027*r")
end

function reset_computer()
    print("\027*x")
end

function mouse_enable(enable)
    print("\27*" .. enable and 1 or 0 .. "m")
end

function mouse_movements(enable)
    print("\27*" .. enable and 1 or 0 .. "n")
end

if #arg ~= 1 then
    io.stderr:write(arg[0] .. " INPUT_FILE")
    os.exit(1)
end

dofile(arg[1])
