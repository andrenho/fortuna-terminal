#!/usr/bin/env lua5.4

local ftclient = require "ftclient"

N_SPRITES = 128
local sprites = {}

-- local ft = ftclient.tcpip("::1")
-- local ft = ftclient.tcpip("127.0.0.1")
local ft = ftclient.tcpip("localhost")
-- local ft = ftclient.tcpip("192.168.4.46")
ft:graphics(true)
ft:print("\27[1;1H\27[2J")
ft:image_load("heavy.png")

for i = 1,N_SPRITES do
    local x = math.random(0, 256 - 16)
    local y = math.random(0, 256 - 16)
    ft:sprite(i - 1, x, y, true, false, false, 0, (i - 1) % 16)
    sprites[i] = { x=x, y=y, dir_x=math.random(-4,4), dir_y=math.random(-4, 4) }
end

ft:enable_vsync(true)

while true do
    local e = ft:poll_event()
    if e.type == 'vsync' then
        for i = 1,N_SPRITES do
            local s = sprites[i]
            s.x = s.x + s.dir_x
            s.y = s.y + s.dir_y
            if s.x < 0 or s.x >= 256 - 16 then s.dir_x = s.dir_x * -1 end
            if s.y < 0 or s.y >= 256 - 16 then s.dir_y = s.dir_y * -1 end
            ft:sprite(i-1, sprites[i].x, sprites[i].y)
        end
    end
end
