--[[
-- configuration.lua
-- Part of Mánagarmr project
-- 29. 11. 2019.
--]]

window =
{
	width  = 800,
	height = 600,
	title  = "The Mánagarmr project.",
	vsync  = 1
}

selection_colour = {0, 255, 0, 42}

--[[
-- Keybind examples
--]]
bind('f10', "exit()")
bind('w', "close()")
bind('space', "toggletargets()")
