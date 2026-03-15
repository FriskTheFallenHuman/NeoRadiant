-- export_obj.lua
-- Exports selected geometry to Wavefront OBJ format

__commandName__        = "objExport"
__commandDisplayName__ = "Export OBJ..."

local function execute()
    local script  = "NeoRadiant Wavefront OBJ Export (*.obj)"
    local version = "0.2"

    local selectionInfo = GlobalSelectionSystem:getSelectionInfo()
    if selectionInfo.totalCount == 0 or
       selectionInfo.totalCount == selectionInfo.componentCount then
        GlobalDialogManager:createMessageBox(
            "No selection", "Nothing selected, cannot run exporter.", Dialog.ERROR):run()
        return
    end

    -- A collected geometry object
    local geomlist = {
        vertexCount = 0,   -- global 1-based OBJ vertex index
        objects     = {},
    }

    local function newGeometry(name)
        return { name = name, vertices = {}, texcoords = {}, faces = {} }
    end

    local function processBrush(brush)
        local geometry   = newGeometry("Brush" .. tostring(#geomlist.objects))
        local exportCaulk = tonumber(GlobalRegistry:get("user/scripts/objExport/exportcaulk")) or 0

        for index = 1, brush:getNumFaces() do
            local face   = brush:getFace(index)
            local shader = face:getShader()
            if shader == "textures/common/caulk" and exportCaulk == 0 then
                -- skip
            else
                local winding    = face:getWinding()
                local firstVertex = geomlist.vertexCount + 1  -- OBJ 1-based

                for _, pt in ipairs(winding) do
                    table.insert(geometry.vertices,  pt.vertex)
                    table.insert(geometry.texcoords, pt.texcoord)
                    geomlist.vertexCount = geomlist.vertexCount + 1
                end

                -- Reversed face list for correct normals
                local faceIndices = {}
                for i = geomlist.vertexCount, firstVertex, -1 do
                    table.insert(faceIndices, i)
                end
                table.insert(geometry.faces, faceIndices)
            end
        end

        print(string.format("Processed brush geometry: %d verts and %d faces",
            #geometry.vertices, #geometry.faces))
        table.insert(geomlist.objects, geometry)
    end

    local function processPatch(patch)
        local shader = patch:getShader()
        local exportCaulk = tonumber(GlobalRegistry:get("user/scripts/objExport/exportcaulk")) or 0
        if shader == "textures/common/caulk" and exportCaulk == 0 then return end

        local geometry    = newGeometry("Patch" .. tostring(#geomlist.objects))
        local mesh        = patch:getTesselatedPatchMesh()
        local firstVertex = geomlist.vertexCount + 1

        for h = 0, mesh.height - 1 do
            for w = 0, mesh.width - 1 do
                local pt = mesh.vertices[h * mesh.width + w + 1]
                table.insert(geometry.vertices,  pt.vertex)
                table.insert(geometry.texcoords, pt.texcoord)
                geomlist.vertexCount = geomlist.vertexCount + 1

                if h > 0 and w > 0 then
                    local base = firstVertex - 1
                    local v1 = base + h * mesh.width + w + 1
                    local v2 = base + (h-1) * mesh.width + w + 1
                    local v3 = base + (h-1) * mesh.width + (w-1) + 1
                    local v4 = base + h * mesh.width + (w-1) + 1
                    table.insert(geometry.faces, {v1, v4, v3, v2})
                end
            end
        end

        print(string.format("Processed patch geometry: %d verts and %d faces",
            #geometry.vertices, #geometry.faces))
        table.insert(geomlist.objects, geometry)
    end

    GlobalSelectionSystem:foreachSelected(function(node)
        if node:isBrush() then
            processBrush(node:getBrush())
        elseif node:isPatch() then
            processPatch(node:getPatch())
        elseif node:isEntity() then
            node:traverseChildren(function(child)
                if child:isBrush() then processBrush(child:getBrush())
                elseif child:isPatch() then processPatch(child:getPatch()) end
                return true
            end)
        else
            print("WARNING: unsupported node type. Skipping: " .. node:getNodeType())
        end
    end)

    -- Dialog
    local dialog = GlobalDialogManager:createDialog(script .. " v" .. version)
    local fileHandle   = dialog:addEntryBox("Filename:")
    dialog:setElementValue(fileHandle, GlobalRegistry:get("user/scripts/objExport/recentFilename"))
    local pathHandle   = dialog:addPathEntry("Save path:", true)
    dialog:setElementValue(pathHandle, GlobalRegistry:get("user/scripts/objExport/recentPath"))
    local centerHandle = dialog:addCheckbox("Center objects at 0,0,0 origin")
    dialog:setElementValue(centerHandle, GlobalRegistry:get("user/scripts/objExport/centerObjects"))
    local caulkHandle  = dialog:addCheckbox("Export caulked faces")
    dialog:setElementValue(caulkHandle, GlobalRegistry:get("user/scripts/objExport/exportcaulk"))

    if dialog:run() ~= Dialog.OK then return end

    local fullpath = dialog:getElementValue(pathHandle) .. "/"
                  .. dialog:getElementValue(fileHandle)
    if not fullpath:match("%.obj$") then fullpath = fullpath .. ".obj" end

    GlobalRegistry:set("user/scripts/objExport/recentFilename", dialog:getElementValue(fileHandle))
    GlobalRegistry:set("user/scripts/objExport/recentPath",     dialog:getElementValue(pathHandle))
    GlobalRegistry:set("user/scripts/objExport/centerObjects",  dialog:getElementValue(centerHandle))
    GlobalRegistry:set("user/scripts/objExport/exportcaulk",    dialog:getElementValue(caulkHandle))

    local overwrite = true
    local f = io.open(fullpath, "r")
    if f then
        f:close()
        local prompt = GlobalDialogManager:createMessageBox("Warning",
            "The file " .. fullpath .. " already exists. Do you wish to overwrite it?", Dialog.ASK)
        overwrite = (prompt:run() == Dialog.YES)
    end
    if not overwrite then return end

    -- Center objects
    if tonumber(dialog:getElementValue(centerHandle)) == 1 then
        local xmin, xmax, ymin, ymax, zmin, zmax =
            math.huge, -math.huge, math.huge, -math.huge, math.huge, -math.huge
        for _, item in ipairs(geomlist.objects) do
            for _, vert in ipairs(item.vertices) do
                if vert:x() < xmin then xmin = vert:x() end; if vert:x() > xmax then xmax = vert:x() end
                if vert:y() < ymin then ymin = vert:y() end; if vert:y() > ymax then ymax = vert:y() end
                if vert:z() < zmin then zmin = vert:z() end; if vert:z() > zmax then zmax = vert:z() end
            end
        end
        local xc = (xmax + xmin) / 2
        local yc = (ymax + ymin) / 2
        local zc = (zmax + zmin) / 2
        for _, item in ipairs(geomlist.objects) do
            for i, vert in ipairs(item.vertices) do
                item.vertices[i] = vert - Vector3(xc, yc, zc)
            end
        end
    end

    -- Write OBJ
    local lines = {}
    for _, x in ipairs(geomlist.objects) do
        table.insert(lines, string.format("g %s\n", x.name))
        for _, vert in ipairs(x.vertices) do
            table.insert(lines, string.format("v %s %s %s\n",
                tostring(vert:x()), tostring(vert:y()), tostring(vert:z())))
        end
        table.insert(lines, "\n")
        for _, tc in ipairs(x.texcoords) do
            table.insert(lines, string.format("vt %s %s\n",
                tostring(tc:x()), tostring(1 - tc:y())))
        end
        table.insert(lines, "\n")
        for _, faceIndices in ipairs(x.faces) do
            local s = "f"
            for _, fi in ipairs(faceIndices) do
                s = s .. string.format(" %d/%d", fi, fi)
            end
            table.insert(lines, s .. "\n")
        end
        table.insert(lines, "\n")
    end

    local out = io.open(fullpath, "w")
    if not out then
        GlobalDialogManager:createMessageBox("Error",
            "Could not open file for writing: " .. fullpath, Dialog.ERROR):run()
        return
    end
    out:write(table.concat(lines))
    out:close()
    print("Done writing OBJ data to " .. fullpath)
end

if __executeCommand__ then
    execute()
end
