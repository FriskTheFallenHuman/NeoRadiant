-- test_targets.lua
-- Lua port of test_targets.py
-- Finds entities with "target" spawnargs pointing to missing entities.

__commandName__        = "test_targets"
__commandDisplayName__ = "Test for Missing Targets"

local function execute()
    local g_targets = {}   -- entityname -> list of targets
    local g_missing = {}   -- entityname -> list of missing targets

    -- Walk the scene and collect all entity names + their targets
    GlobalSceneGraph:root():traverse(function(node)
        if node:isEntity() then
            local name  = node:getKeyValue("name")
            local targs = {}
            local pairs = node:getKeyValuePairs("target")
            for _, kv in ipairs(pairs) do
                table.insert(targs, kv[2])
            end
            g_targets[name] = targs
        end
        return true
    end)

    -- Find missing targets
    local targetcount = 0
    for name, targs in pairs(g_targets) do
        targetcount = targetcount + #targs
        local missing = {}
        for _, targ in ipairs(targs) do
            if not g_targets[targ] then
                table.insert(missing, targ)
            end
        end
        if #missing > 0 then
            g_missing[name] = missing
        end
    end

    -- Build report
    local entitycount = 0
    for _ in pairs(g_targets) do entitycount = entitycount + 1 end

    local msg = string.format("%d entities found with %d targets\n\n",
                              entitycount, targetcount)

    local missingCount = 0
    for _ in pairs(g_missing) do missingCount = missingCount + 1 end

    if missingCount == 0 then
        msg = msg .. "No missing targets found"
        GlobalDialogManager:createMessageBox(
            "Missing targets", msg, Dialog.CONFIRM):run()
    else
        msg = msg .. "Missing targets:\n"
        for ent, targs in pairs(g_missing) do
            for _, targ in ipairs(targs) do
                msg = msg .. ent .. " -> " .. targ .. "\n"
            end
        end
        print(msg)
        msg = msg .. "\nThe list of missing targets has been printed to the console."
        msg = msg .. "\n\nDo you want to select all entities with missing targets?"

        local response = GlobalDialogManager:createMessageBox(
            "Missing targets", msg, Dialog.ASK):run()

        if response == Dialog.YES then
            GlobalSceneGraph:root():traverse(function(node)
                if node:isEntity() then
                    local name = node:getKeyValue("name")
                    if g_missing[name] then
                        node:setSelected(true)
                    end
                end
                return true
            end)
        end
    end
end

if __executeCommand__ then
    execute()
end
