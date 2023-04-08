#!/usr/bin/env lua5.4

local ftclient = require "ftclient"

-- local ft = ftclient.tcpip("localhost")
local ft = ftclient.tcpip("127.0.0.1")
ft:print("Hello world!")

ft:mouse_enable(true)

while true do
    local e = ft:poll_event()
    if e then
        if e.type == 'keypress' then
            print(">> " .. e.key .. " <<")
        elseif e.type == 'mousepress' then
            print(e.button, e.pos_x, e.pos_y)
        end
    end
end
