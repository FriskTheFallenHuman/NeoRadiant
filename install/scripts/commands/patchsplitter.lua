-- patchsplitter.lua
-- Splits a patch along a selected row or column

__commandName__        = "SplitPatch"
__commandDisplayName__ = "Split patch"

local function execute()
    local MAXGRIDPOWER =  8   -- Grid 256
    local MINGRIDPOWER = -1   -- Grid 0.5

    -- ------------------------------------------------------------------ helpers

    local function getAndValidateSelection()
        local sInfo = GlobalSelectionSystem:getSelectionInfo()
        if sInfo.patchCount ~= 1 or sInfo.brushCount > 0 or sInfo.entityCount > 0
           or sInfo.componentCount < 2 then
            error("Bad selection. Select one patch only, and 2 or more verts from the same (pink) row or column.")
        end
        local patch = GlobalSelectionSystem:ultimateSelected():getPatch()
        if not patch:isValid() then
            error("This isn't a valid patch.")
        end
        return patch
    end

    -- Store all data needed to reconstruct a patch exactly
    local function capturePatchData(patch)
        local data = {
            cols             = patch:getWidth(),
            rows             = patch:getHeight(),
            shader           = patch:getShader(),
            subdivisionsFixed = patch:subdivionsFixed(),
            subdivisions     = patch:getSubdivisions(),
            verts            = {},
        }
        for row = 0, data.rows - 1 do
            data.verts[row] = {}
            for col = 0, data.cols - 1 do
                local ctrl = patch:ctrlAt(row, col)
                data.verts[row][col] = {
                    vertex   = Vector3(ctrl.vertex),
                    texcoord = Vector2(ctrl.texcoord),
                }
            end
        end
        return data
    end

    local function resetPatch(patch, pd)
        patch:setDims(pd.cols, pd.rows)
        patch:setShader(pd.shader)
        patch:setFixedSubdivisions(pd.subdivisionsFixed, pd.subdivisions)
        for row = 0, pd.rows - 1 do
            for col = 0, pd.cols - 1 do
                local ctrl = patch:ctrlAt(row, col)
                ctrl.vertex   = pd.verts[row][col].vertex
                ctrl.texcoord = pd.verts[row][col].texcoord
            end
        end
        patch:controlPointsChanged()
    end

    local function clonePatch(patch)
        patch:setSelected(false)
        patch:setSelected(true)
        GlobalCommandSystem:execute("CloneSelection")
        return GlobalSelectionSystem:ultimateSelected():getPatch()
    end

    -- Sub-range helpers: return a new patchdata table covering rows [first..last)
    local function getRows(pd, first, last)
        last = last or pd.rows
        local nd = { cols = pd.cols, rows = last - first,
                     shader = pd.shader,
                     subdivisionsFixed = pd.subdivisionsFixed,
                     subdivisions = pd.subdivisions,
                     verts = {} }
        for row = first, last - 1 do
            nd.verts[row - first] = {}
            for col = 0, pd.cols - 1 do
                nd.verts[row - first][col] = pd.verts[row][col]
            end
        end
        return nd
    end

    local function getCols(pd, first, last)
        last = last or pd.cols
        local nd = { cols = last - first, rows = pd.rows,
                     shader = pd.shader,
                     subdivisionsFixed = pd.subdivisionsFixed,
                     subdivisions = pd.subdivisions,
                     verts = {} }
        for row = 0, pd.rows - 1 do
            nd.verts[row] = {}
            for col = first, last - 1 do
                nd.verts[row][col - first] = pd.verts[row][col]
            end
        end
        return nd
    end

    -- ------------------------------------------------------------------ vert detection

    local function distToEdge(patch, direction, axis, gridPower)
        GlobalGrid:setGridSize(gridPower)
        local stepsize = 2 ^ gridPower
        local reverse  = direction == "left" and "right" or direction == "right" and "left"
                      or direction == "up"   and "down"  or "up"

        local function origin()
            local aabb = patch:getWorldAABB()
            if axis == "x" then return aabb.origin:x() end
            if axis == "y" then return aabb.origin:y() end
            return aabb.origin:z()
        end
        local function extent()
            local aabb = patch:getWorldAABB()
            if axis == "x" then return aabb.extents:x() end
            if axis == "y" then return aabb.extents:y() end
            return aabb.extents:z()
        end

        local isRight = direction == "right" or direction == "up"
        local startBoundary = isRight and (origin() + extent()) or (origin() - extent())
        local looplimit = (extent() * 2) / stepsize

        local stepcount = 0
        while stepcount < looplimit do
            local boundary = isRight and (origin() + extent()) or (origin() - extent())
            if isRight then
                if boundary < startBoundary then break end
            else
                if boundary > startBoundary then break end
            end
            GlobalCommandSystem:execute("SelectNudge" .. direction)
            stepcount = stepcount + 1
        end
        for _ = 1, stepcount do
            GlobalCommandSystem:execute("SelectNudge" .. reverse)
        end
        return (stepcount - 1) * stepsize
    end

    local TooManyVerts = {}
    local TooFewVerts  = {}

    local function attemptGetVertsLine(patch, tolerance)
        local gridPower = math.floor(math.log(tolerance) / math.log(2))
        local AABB      = patch:getWorldAABB()

        GlobalCommandSystem:execute("ViewSide")
        local minX = AABB.origin:x() - AABB.extents:x() + distToEdge(patch,"left","x",gridPower) - tolerance
        local maxX = AABB.origin:x() + AABB.extents:x() - distToEdge(patch,"right","x",gridPower) + tolerance
        local minZ = AABB.origin:z() - AABB.extents:z() + distToEdge(patch,"down","z",gridPower) - tolerance
        local maxZ = AABB.origin:z() + AABB.extents:z() - distToEdge(patch,"up","z",gridPower) + tolerance

        GlobalCommandSystem:execute("ViewFront")
        local minY = AABB.origin:y() - AABB.extents:y() + distToEdge(patch,"left","y",gridPower) - tolerance
        local maxY = AABB.origin:y() + AABB.extents:y() - distToEdge(patch,"right","y",gridPower) + tolerance

        GlobalCommandSystem:execute("ViewTop")

        local includedRows = {}
        local includedCols = {}
        for row = 0, patch:getHeight() - 1 do
            for col = 0, patch:getWidth() - 1 do
                local ctrl = patch:ctrlAt(row, col)
                local v    = ctrl.vertex
                if minX <= v:x() and v:x() <= maxX and
                   minY <= v:y() and v:y() <= maxY and
                   minZ <= v:z() and v:z() <= maxZ then
                    includedRows[row] = true
                    includedCols[col] = true
                end
            end
        end

        local nRows, nCols = 0, 0
        for _ in pairs(includedRows) do nRows = nRows + 1 end
        for _ in pairs(includedCols) do nCols = nCols + 1 end

        -- Special case: seam
        local hasFirst = includedRows[0] or includedRows[patch:getHeight()-1]
        local hasLast  = includedCols[0] or includedCols[patch:getWidth()-1]
        if (includedRows[0] and includedRows[patch:getHeight()-1]) or
           (includedCols[0] and includedCols[patch:getWidth()-1]) then
            error("You've selected the existing seam of a 3d patch.")
        end

        if nRows > 1 and nCols > 1 then error(TooManyVerts) end
        if nRows < 2 and nCols < 2 then error(TooFewVerts) end

        if nRows == 1 then
            for row in pairs(includedRows) do return "row", row end
        else
            for col in pairs(includedCols) do return "col", col end
        end
    end

    local function getSelectedVertsLine(patch, patchdata)
        local userGridPower = GlobalGrid:getGridPower()
        local tolerance     = 2 ^ userGridPower
        local lineType, lineNum = nil, nil

        while not lineType and tolerance >= 2^MINGRIDPOWER and tolerance <= 2^MAXGRIDPOWER do
            print(string.format("Patch Splitter: trying tolerance %.2f", tolerance))
            local ok, res1, res2 = pcall(attemptGetVertsLine, patch, tolerance)
            if ok then
                lineType, lineNum = res1, res2
            elseif res1 == TooManyVerts then
                tolerance = tolerance / 2
            else
                tolerance = tolerance * 2
            end
        end

        GlobalGrid:setGridSize(userGridPower)
        resetPatch(patch, patchdata)

        if not lineType then
            error("Unable to determine selected verts. Try again with different verts.")
        end
        if lineNum == 0 or
           (lineType == "row" and lineNum == patch:getHeight() - 1) or
           (lineType == "col" and lineNum == patch:getWidth()  - 1) then
            error("You've selected the existing edge of the patch.")
        end
        if lineNum % 2 ~= 0 then
            error("You've selected a green line. Patches can only be cut along pink lines.")
        end
        return lineType, lineNum
    end

    -- ------------------------------------------------------------------ main

    local patch     = getAndValidateSelection()
    local patchdata = capturePatchData(patch)

    local lineType, lineNum = getSelectedVertsLine(patch, patchdata)
    print("RESULT: " .. lineType .. " " .. tostring(lineNum))

    local newpatch = clonePatch(patch)
    local ok, err  = pcall(function()
        local newdata1, newdata2
        if lineType == "row" then
            newdata1 = getRows(patchdata, 0,       lineNum + 1)
            newdata2 = getRows(patchdata, lineNum,  patchdata.rows)
        else
            newdata1 = getCols(patchdata, 0,       lineNum + 1)
            newdata2 = getCols(patchdata, lineNum,  patchdata.cols)
        end
        resetPatch(newpatch, newdata2)
        resetPatch(patch,    newdata1)
    end)

    if not ok then
        GlobalSelectionSystem:setSelectedAll(false)
        newpatch:setSelected(true)
        GlobalCommandSystem:execute("deleteSelected")
        resetPatch(patch, patchdata)
        error(err)
    end

    patch:setSelected(true)
    newpatch:setSelected(true)
end

if __executeCommand__ then
    local ok, err = pcall(execute)
    if not ok then
        GlobalDialogManager:createMessageBox("Patch Splitter", tostring(err), Dialog.ERROR):run()
    end
end
