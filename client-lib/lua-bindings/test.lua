local ftclient = require "ftclient"

local ft = ftclient.tcpip("localhost")
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