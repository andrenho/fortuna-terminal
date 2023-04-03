local ftclient = require "ftclient"

local ft = ftclient.new()
print(ft:image_load("test.png"))