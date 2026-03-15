-- shift_textures_randomly.lua
-- Lua port of shift_textures_randomly.py

__commandName__        = "ShiftTexturesRandomly"
__commandDisplayName__ = "Shift Textures randomly"

local function execute()
    math.randomseed(os.time())

    GlobalSelectionSystem:foreachSelectedFace(function(face)
        local s = math.random(0, 100) / 100
        local t = math.random(0, 100) / 100
        face:shiftTexdef(s, t)
    end)

    GlobalSelectionSystem:foreachSelected(function(node)
        if node:isPatch() then
            local patch = node:getPatch()
            local s = math.random(0, 100)
            local t = math.random(0, 100)
            patch:translateTexture(s, t)
        end
    end)

    GlobalCameraManager:getActiveView():refresh()
end

if __executeCommand__ then
    execute()
end
