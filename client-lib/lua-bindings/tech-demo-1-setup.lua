#!/usr/bin/env lua5.4

local ftclient = require "ftclient"

N_SPRITES = 128
local sprites = {}

local ft = ftclient.new()
print(ft:graphics(true))
print(ft:print("\27[1;1H\27[2J"))
print(ft:image_load("heavy.png"))
print(ft:enable_vsync(true))
