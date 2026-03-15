-- check_for_invalid_visportals.lua

__commandName__        = "check_invalid_visportals"
__commandDisplayName__ = "Test for invalid Visportals"

local function execute()
    local visportalShader = "textures/editor/visportal"
    local numPortals  = 0
    local invalidList = {}

    GlobalSceneGraph:root():traverse(function(node)
        if node:isBrush() then
            local brush = node:getBrush()
            if brush:hasShader(visportalShader) then
                numPortals = numPortals + 1

                -- Count faces that use the visportal shader
                local count = 0
                for i = 1, brush:getNumFaces() do
                    if brush:getFace(i):getShader() == visportalShader then
                        count = count + 1
                    end
                end

                if count ~= 1 then
                    table.insert(invalidList, node)
                end
            end
            return false   -- don't traverse brush children
        end
        return true
    end)

    local msg = numPortals .. " visportals checked, " ..
                #invalidList .. " have errors\n"

    if numPortals == 0 then
        msg = "There are no visportals in this map."
    end

    if #invalidList > 0 then
        GlobalSelectionSystem:setSelectedAll(false)
        for _, node in ipairs(invalidList) do
            node:setSelected(true)
        end
        msg = msg .. "The problematic visportals have been highlighted."
    end

    GlobalDialogManager:createMessageBox("Visportal Test Results", msg,
                                         Dialog.CONFIRM):run()
end

if __executeCommand__ then
    execute()
end
