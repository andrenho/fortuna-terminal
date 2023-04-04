local ftclient = require "ftclient"

local ft = ftclient.tcpip("localhost")
-- local ft = ftclient.new()
print(ft:print("Hello world!"))
-- io.stdin:read'*l'