local ftclient = require "ftclient"

N_SPRITES = 128
local sprites = {}

local ft = ftclient.tcpip("localhost")
ft:graphics(true)
ft:print("\27[1;1H\27[2J")
ft:image_load("heavy.png")

for i = 1,N_SPRITES do
    local x = math.random(0, 255)
    local y = math.random(0, 255)
    ft:sprite(i - 1, x, y, true, false, false, 0, (i - 1) % 16)
    sprites[i] = { x=x, y=y, dir_x=math.random(-1,1), dir_y=math.random(-1, 1) }
end

ft:enable_vsync(true)

while true do
    local e = ft:poll_event()
    if e.type == 'vsync' then
        for i = 1,N_SPRITES do
            sprites[i].x = sprites[i].x + sprites[i].dir_x
            sprites[i].y = sprites[i].y + sprites[i].dir_y
            ft:sprite(i-1, sprites[i].x, sprites[i].y)
        end
    end
end
