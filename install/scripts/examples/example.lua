-- example.lua
-- Demonstrates the shader system and dialog API

__commandName__        = "Example"
__commandDisplayName__ = "Nice display name for the menus"

local function execute()
    local shader = GlobalMaterialManager:getMaterialForName("bc_rat")
    print(shader:getName())

    GlobalDialogManager:createMessageBox("Example Box",
        "Shader loaded: " .. shader:getName(), Dialog.CONFIRM):run()
end

if __executeCommand__ then
    execute()
end
