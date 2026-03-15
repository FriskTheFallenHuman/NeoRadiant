-- select_all_models_of_type.lua
-- Lua port of select_all_models_of_type.py

__commandName__        = "SelectAllModelsOfType"
__commandDisplayName__ = "Select all Models of same type"

local function execute()
    local selectedModelNames = {}

    -- Collect model names from the current selection
    GlobalSelectionSystem:foreachSelected(function(node)
        if node:isEntity() then
            local m = node:getKeyValue("model")
            if m ~= "" then
                selectedModelNames[m] = true
            end
        end
    end)

    print("Unique models currently selected: " .. tostring(#selectedModelNames))

    -- Now traverse the scene and select matching nodes
    GlobalSceneGraph:root():traverse(function(node)
        if node:isEntity() then
            local modelName = node:getKeyValue("model")
            if modelName ~= "" and selectedModelNames[modelName] then
                node:setSelected(true)
            end
            return false  -- don't traverse entity children
        end
        return true
    end)
end

if __executeCommand__ then
    execute()
end
