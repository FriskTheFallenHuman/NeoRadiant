-- shift_textures_upwards_randomly.lua
-- Lua port of shift_textures_upwards_randomly.py

__commandName__        = "ShiftTexturesUpwardsRandomly"
__commandDisplayName__ = "Shift Textures randomly upwards"

local function execute()
    math.randomseed(os.time())

    GlobalSelectionSystem:foreachSelectedFace(function(face)
        local t = math.random(0, 100) / 100
        face:shiftTexdef(0, t)
    end)

    GlobalSelectionSystem:foreachSelected(function(node)
        if node:isPatch() then
            local patch = node:getPatch()
            local t = math.random(0, 100)
            patch:translateTexture(0, t)
        end
    end)

    GlobalCameraManager:getActiveView():refresh()
end

if __executeCommand__ then
    execute()
end
