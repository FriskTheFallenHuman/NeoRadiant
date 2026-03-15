-- find_duplicate_entities.lua

__commandName__        = "findDuplicateEntities"
__commandDisplayName__ = "Find Duplicate Entities"

if __executeCommand__ then
    local entities = {}

    GlobalSelectionSystem:setSelectedAll(false)

    GlobalSceneGraph:root():traverse(function(node)
        if node:isEntity() then
            table.insert(entities, {
                node      = node,
                origin    = node:getKeyValue("origin"),
                model     = node:getKeyValue("model"),
                classname = node:getKeyValue("classname"),
                rotation  = node:getKeyValue("rotation"),
            })
            return false  -- don't traverse entity children
        end
        return true
    end)

    local duplicates = 0
    local n = #entities
    for i = 1, n - 1 do
        local a = entities[i]
        for j = i + 1, n do
            local b = entities[j]
            if a.origin    == b.origin    and
               a.model     == b.model     and
               a.classname == b.classname and
               a.rotation  == b.rotation  then
                b.node:setSelected(true)
                duplicates = duplicates + 1
            end
        end
    end

    local result
    if duplicates > 0 then
        result = string.format("Found and selected %d duplicate entities.", duplicates)
    else
        result = "No duplicate entities found."
    end

    GlobalDialogManager:createMessageBox(
        "Find Duplicate Entities Results", result, Dialog.CONFIRM):run()
end
