-- ase_export.lua
-- Exports selected geometry to ASE format

__commandName__        = "aseExport"
__commandDisplayName__ = "Export ASE..."

local function execute()
    local script  = "NeoRadiant ASCII Scene Export (*.ase)"
    local version = "0.9"

    local selectionInfo = GlobalSelectionSystem:getSelectionInfo()
    if selectionInfo.totalCount == 0 or
       selectionInfo.totalCount == selectionInfo.componentCount then
        GlobalDialogManager:createMessageBox(
            "No selection", "Nothing selected, cannot run exporter.", Dialog.ERROR):run()
        return
    end

    local shaderlist = {}
    local geomlist   = {}

    -- Helper: find index of value in list (1-based), or append and return new index
    local function shaderIndex(shader)
        for i, s in ipairs(shaderlist) do
            if s == shader then return i end
        end
        table.insert(shaderlist, shader)
        return #shaderlist
    end

    -- Simple linear triangulation of n-sided poly (0-based indices into local verts)
    local function triangulate(pointset)
        local tris = {}
        for count = 2, #pointset do
            table.insert(tris, { pointset[1], pointset[count - 1], pointset[count] })
        end
        return tris
    end

    -- Skin patch matrix with tris
    local function skinmatrix(pointset, width, height)
        local tris = {}
        for h = 0, height - 2 do
            for w = 0, width - 2 do
                local i = function(row, col) return pointset[row * width + col + 1] end
                table.insert(tris, { i(h,w),   i(h,w+1),   i(h+1,w)   })
                table.insert(tris, { i(h,w+1),  i(h+1,w+1), i(h+1,w)   })
            end
        end
        return tris
    end

    local function processBrush(brush)
        local verts = {}
        local faces = {}
        local exportCaulk = tonumber(GlobalRegistry:get("user/scripts/aseExport/exportcaulk")) or 0
        local numfaces = brush:getNumFaces()
        for index = 1, numfaces do
            local face   = brush:getFace(index)
            local shader = face:getShader()
            if shader == "textures/common/caulk" and exportCaulk == 0 then
                -- skip
            else
                local sidx    = shaderIndex(shader)
                local winding = face:getWinding()
                -- build index list for this face (1-based into local verts)
                local baseIdx = #verts
                local idxList = {}
                for j = 1, #winding do
                    idxList[j] = baseIdx + j
                end
                local tris = triangulate(idxList)
                for _, tri in ipairs(tris) do
                    table.insert(faces, { tri[1], tri[2], tri[3], sidx })
                end
                -- reversed winding
                for j = #winding, 1, -1 do
                    local w = winding[j]
                    table.insert(verts, {
                        w.vertex:x(),  w.vertex:y(),  w.vertex:z(),
                        w.texcoord:x(), w.texcoord:y() * -1,
                        w.normal:x(),  w.normal:y(),  w.normal:z(),
                    })
                end
            end
        end
        table.insert(geomlist, { verts, faces })
    end

    local function processPatch(patch)
        local verts  = {}
        local faces  = {}
        local shader = patch:getShader()
        local exportCaulk = tonumber(GlobalRegistry:get("user/scripts/aseExport/exportcaulk")) or 0
        if shader == "textures/common/caulk" and exportCaulk == 0 then return end
        local sidx = shaderIndex(shader)
        local mesh = patch:getTesselatedPatchMesh()
        for _, x in ipairs(mesh.vertices) do
            table.insert(verts, {
                x.vertex:x(),  x.vertex:y(),  x.vertex:z(),
                x.texcoord:x(), x.texcoord:y() * -1,
                x.normal:x(),  x.normal:y(),  x.normal:z(),
            })
        end
        local idxList = {}
        for i = 1, #verts do idxList[i] = i end
        local tris = skinmatrix(idxList, mesh.width, mesh.height)
        for _, tri in ipairs(tris) do
            table.insert(faces, { tri[1], tri[2], tri[3], sidx })
        end
        table.insert(geomlist, { verts, faces })
    end

    local function processPrimitive(node)
        if node:isBrush() then
            processBrush(node:getBrush())
        elseif node:isPatch() then
            processPatch(node:getPatch())
        end
    end

    GlobalSelectionSystem:foreachSelected(function(node)
        if node:isBrush() or node:isPatch() then
            processPrimitive(node)
        elseif node:isEntity() then
            node:traverseChildren(function(child)
                if child:isBrush() then
                    processBrush(child:getBrush())
                elseif child:isPatch() then
                    processPatch(child:getPatch())
                end
                return true
            end)
        else
            print("WARNING: unsupported node type selected. Skipping: " .. node:getNodeType())
        end
    end)

    -- Dialog
    local dialog = GlobalDialogManager:createDialog(script .. " v" .. version)
    local fileHandle   = dialog:addEntryBox("Filename:")
    dialog:setElementValue(fileHandle,
        GlobalRegistry:get("user/scripts/aseExport/recentFilename"))
    local pathHandle   = dialog:addPathEntry("Save path:", true)
    dialog:setElementValue(pathHandle,
        GlobalRegistry:get("user/scripts/aseExport/recentPath"))
    local centerHandle = dialog:addCheckbox("Center objects at 0,0,0 origin")
    dialog:setElementValue(centerHandle,
        GlobalRegistry:get("user/scripts/aseExport/centerObjects"))
    local caulkHandle  = dialog:addCheckbox("Export caulked faces")
    dialog:setElementValue(caulkHandle,
        GlobalRegistry:get("user/scripts/aseExport/exportcaulk"))

    if dialog:run() ~= Dialog.OK then return end

    local fullpath = dialog:getElementValue(pathHandle) .. "/"
                  .. dialog:getElementValue(fileHandle)
    if not fullpath:match("%.ase$") then fullpath = fullpath .. ".ase" end

    GlobalRegistry:set("user/scripts/aseExport/recentFilename",
                       dialog:getElementValue(fileHandle))
    GlobalRegistry:set("user/scripts/aseExport/recentPath",
                       dialog:getElementValue(pathHandle))
    GlobalRegistry:set("user/scripts/aseExport/centerObjects",
                       dialog:getElementValue(centerHandle))
    GlobalRegistry:set("user/scripts/aseExport/exportcaulk",
                       dialog:getElementValue(caulkHandle))

    -- Check overwrite
    local overwrite = true
    local f = io.open(fullpath, "r")
    if f then
        f:close()
        local prompt = GlobalDialogManager:createMessageBox("Warning",
            "The file " .. fullpath .. " already exists. Do you wish to overwrite it?",
            Dialog.ASK)
        overwrite = (prompt:run() == Dialog.YES)
    end
    if not overwrite then return end

    -- Center objects
    if tonumber(dialog:getElementValue(centerHandle)) == 1 then
        local xmin, xmax, ymin, ymax, zmin, zmax = math.huge, -math.huge,
            math.huge, -math.huge, math.huge, -math.huge
        for _, item in ipairs(geomlist) do
            for _, vert in ipairs(item[1]) do
                if vert[1] < xmin then xmin = vert[1] end
                if vert[1] > xmax then xmax = vert[1] end
                if vert[2] < ymin then ymin = vert[2] end
                if vert[2] > ymax then ymax = vert[2] end
                if vert[3] < zmin then zmin = vert[3] end
                if vert[3] > zmax then zmax = vert[3] end
            end
        end
        local xc = (xmax + xmin) / 2
        local yc = (ymax + ymin) / 2
        local zc = (zmax + zmin) / 2
        for _, item in ipairs(geomlist) do
            for _, vert in ipairs(item[1]) do
                vert[1] = vert[1] - xc
                vert[2] = vert[2] - yc
                vert[3] = vert[3] - zc
            end
        end
    end

    -- Build ASE output
    local lines = {}
    local function w(s) table.insert(lines, s) end

    w(string.format('*3DSMAX_ASCIIEXPORT\t200'))
    w(string.format('*COMMENT "%s v%s"', script, version))
    w('*SCENE {')
    w(string.format('\t*SCENE_FILENAME "%s"', GlobalMap:getMapName()))
    w('\t*SCENE_FIRSTFRAME 0\n\t*SCENE_LASTFRAME 100\n\t*SCENE_FRAMESPEED 30')
    w('\t*SCENE_TICKSPERFRAME 160')
    w('\t*SCENE_BACKGROUND_STATIC 0.0000\t0.0000\t0.0000')
    w('\t*SCENE_AMBIENT_STATIC 0.0000\t0.0000\t0.0000')
    w('}')
    w(string.format('*MATERIAL_LIST {\n\t*MATERIAL_COUNT %d', #shaderlist))
    for idx, shader in ipairs(shaderlist) do
        w(string.format('\t*MATERIAL %d {\n\t\t*MATERIAL_NAME "%s"', idx - 1, shader))
        w('\t\t*MATERIAL_CLASS "Standard"')
        w('\t\t*MATERIAL_AMBIENT 0.5882\t0.5882\t0.5882')
        w('\t\t*MATERIAL_DIFFUSE 0.5882\t0.5882\t0.5882')
        w('\t\t*MATERIAL_SPECULAR 0.9000\t0.9000\t0.9000')
        w('\t\t*MATERIAL_SHINE 0.1000\n\t\t*MATERIAL_SHINESTRENGTH 0.0000')
        w('\t\t*MATERIAL_TRANSPARENCY 0.0000\n\t\t*MATERIAL_WIRESIZE 1.0000')
        w('\t\t*MATERIAL_SHADING Blinn\n\t\t*MATERIAL_XP_FALLOFF 0.0000')
        w('\t\t*MATERIAL_SELFILLUM 0.0000\n\t\t*MATERIAL_FALLOFF In')
        w('\t\t*MATERIAL_XP_TYPE Filter')
        w(string.format('\t\t*MAP_DIFFUSE {\n\t\t\t*MAP_NAME "%s"', shader))
        w('\t\t\t*MAP_CLASS "Bitmap"\n\t\t\t*MAP_SUBNO 1\n\t\t\t*MAP_AMOUNT 1.0000')
        w(string.format('\t\t\t*BITMAP "//base/%s"', shader))
        w('\t\t\t*MAP_TYPE Screen\n\t\t\t*UVW_U_OFFSET 0.0000\n\t\t\t*UVW_V_OFFSET 0.0000')
        w('\t\t\t*UVW_U_TILING 1.0000\n\t\t\t*UVW_V_TILING 1.0000\n\t\t\t*UVW_ANGLE 0.0000')
        w('\t\t\t*UVW_BLUR 1.0000\n\t\t\t*UVW_BLUR_OFFSET 0.0000')
        w('\t\t\t*UVW_NOUSE_AMT 1.0000\n\t\t\t*UVW_NOISE_SIZE 1.0000')
        w('\t\t\t*UVW_NOISE_LEVEL 1\n\t\t\t*UVW_NOISE_PHASE 0.0000')
        w('\t\t\t*BITMAP_FILTER Pyramidal\n\t\t}\n\t}')
    end
    w('}')

    for gi, x in ipairs(geomlist) do
        local verts = x[1]; local faces = x[2]
        if #faces == 0 then goto continue end
        local name = "mesh" .. tostring(gi - 1)
        w(string.format('*GEOMOBJECT {\n\t*NODE_NAME "%s"', name))
        w(string.format('\t*NODE_TM {\n\t\t*NODE_NAME "%s"', name))
        w('\t\t*INHERIT_POS 0 0 0\n\t\t*INHERIT_ROT 0 0 0\n\t\t*INHERIT_SCL 0 0 0')
        w('\t\t*TM_ROW0 1.0000\t0.0000\t0.0000\n\t\t*TM_ROW1 0.0000\t1.0000\t0.0000')
        w('\t\t*TM_ROW2 0.0000\t0.0000\t1.0000\n\t\t*TM_ROW3 0.0000\t0.0000\t0.0000')
        w('\t\t*TM_POS 0.0000\t0.0000\t0.0000\n\t\t*TM_ROTAXIS 0.0000\t0.0000\t0.0000')
        w('\t\t*TM_ROTANGLE 0.0000\n\t\t*TM_SCALE 1.0000\t1.0000\t1.0000')
        w('\t\t*TM_SCALEAXIS 0.0000\t0.0000\t0.0000\n\t\t*TM_SCALEAXISANG 0.0000\n\t}')
        w(string.format('\t*MESH {\n\t\t*TIMEVALUE 0\n\t\t*MESH_NUMVERTEX %d\n\t\t*MESH_NUMFACES %d',
            #verts, #faces))
        w('\t\t*MESH_VERTEX_LIST {')
        for i, v in ipairs(verts) do
            w(string.format('\t\t\t*MESH_VERTEX %d\t%10.4f\t%10.4f\t%10.4f', i-1, v[1], v[2], v[3]))
        end
        w('\t\t}')
        w('\t\t*MESH_FACE_LIST {')
        for i, f in ipairs(faces) do
            w(string.format('\t\t\t*MESH_FACE %d: A: %d B: %d C: %d AB: 0 BC: 0 CA: 0\t*MESH_SMOOTHING 1\t*MESH_MTLID %d',
                i-1, f[1]-1, f[2]-1, f[3]-1, f[4]-1))
        end
        w('\t\t}')
        w(string.format('\t\t*MESH_NUMTVERTEX %d', #verts))
        w('\t\t*MESH_TVERTLIST {')
        for i, v in ipairs(verts) do
            w(string.format('\t\t\t*MESH_TVERT %d\t%10.4f\t%10.4f\t0.0000', i-1, v[4], v[5]))
        end
        w('\t\t}')
        w(string.format('\t\t*MESH_NUMTVFACES %d', #faces))
        w('\t\t*MESH_TFACELIST {')
        for i, f in ipairs(faces) do
            w(string.format('\t\t\t*MESH_TFACE %d\t%d\t%d\t%d', i-1, f[1]-1, f[2]-1, f[3]-1))
        end
        w('\t\t}')
        w('\t\t*MESH_NUMCVERTEX 1\n\t\t*MESH_CVERTLIST {\n\t\t\t*MESH_VERTCOL 0\t1.0000\t1.0000\t1.0000\n\t\t}')
        w(string.format('\t\t*MESH_NUMCVFACES %d', #faces))
        w('\t\t*MESH_CFACELIST {')
        for i = 1, #faces do
            w(string.format('\t\t\t*MESH_CFACE %d\t0\t0\t0', i-1))
        end
        w('\t\t}')
        w('\t\t*MESH_NORMALS {')
        for i, f in ipairs(faces) do
            local v0 = verts[f[1]]
            w(string.format('\t\t\t*MESH_FACENORMAL %d\t%10.4f\t%10.4f\t%10.4f',
                i-1, v0[6], v0[7], v0[8]))
            for _, vi in ipairs({f[1], f[2], f[3]}) do
                local vn = verts[vi]
                w(string.format('\t\t\t\t*MESH_VERTEXNORMAL %d\t%10.4f\t%10.4f\t%10.4f',
                    vi-1, vn[6], vn[7], vn[8]))
            end
        end
        w('\t\t}\n\t}')
        w('\t*PROP_MOTIONBLUR 0\n\t*PROP_CASTSHADOW 1\n\t*PROP_RECVSHADOW 1')
        w(string.format('\t*MATERIAL_REF %d\n}', faces[1][4] - 1))
        ::continue::
    end

    local out = io.open(fullpath, "w")
    if not out then
        GlobalDialogManager:createMessageBox("Error",
            "Could not open file for writing: " .. fullpath, Dialog.ERROR):run()
        return
    end
    out:write(table.concat(lines, "\n") .. "\n")
    out:close()
end

if __executeCommand__ then
    execute()
end
