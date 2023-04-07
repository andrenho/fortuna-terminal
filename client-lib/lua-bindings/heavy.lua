local ftclient = require "ftclient"



local ft = ftclient.tcpip("localhost")
ft:graphics(true)
ft:print("\27[1;1H\27[2J")
ft:image_load("heavy.png")
ft:sprite(0, 20, 20, true, false, false, 0, 0)
ft:sprite(1, 40, 40, true, false, false, 0, 1)
ft:sprite(2, 60, 40, true, false, false, 0, 2)
