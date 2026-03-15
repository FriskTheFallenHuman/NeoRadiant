-- materialtest.lua
-- Lua port of materialtest.py — exercises the material editing API

local function boolStr(v) return v and "can" or "cannot" end

print(string.format("bc_rat %s be modified",
    boolStr(GlobalMaterialManager:materialCanBeModified("bc_rat"))))

local bcRatCopy = GlobalMaterialManager:copyMaterial("bc_rat", "bc_rat_copy")
if bcRatCopy then
    print("Got a copy of bc_rat named " .. bcRatCopy:getName())
    print(string.format("The copy %s be modified",
        boolStr(GlobalMaterialManager:materialCanBeModified(bcRatCopy:getName()))))
else
    print("copyMaterial returned nil (bc_rat is in a PK4 and cannot be copied)")
end

local ok, err = pcall(function()
    local bcRat = GlobalMaterialManager:getMaterial("bc_rat")
    bcRat:setEditorImageExpressionFromString("this is gonna blow")
end)
if not ok then
    print("An exception has been thrown: " .. tostring(err))
end

if bcRatCopy then
    print("Removing the copy again...")
    GlobalMaterialManager:removeMaterial(bcRatCopy:getName())
end

print("Create an empty material named textures/lua/blah...")
local newMaterial = GlobalMaterialManager:createEmptyMaterial("textures/lua/blah")

print(string.format("The new material %s be modified",
    boolStr(GlobalMaterialManager:materialCanBeModified(newMaterial:getName()))))

local renameResult = GlobalMaterialManager:renameMaterial(
    newMaterial:getName(), newMaterial:getName() .. "_renamed")
print("The rename operation " .. (renameResult and "was successful" or "failed"))

print("The existing material reference now has the name " .. newMaterial:getName())

local filename = "materials/_lua_test.mtr"
newMaterial:setShaderFileName(filename)
newMaterial:setEditorImageExpressionFromString("textures/common/caulk")
newMaterial:setSortRequest(45.3)

print("Surface Type: "          .. tostring(newMaterial:getSurfaceType()))
print("Cull type: "             .. tostring(newMaterial:getCullType()))
print("Clamp type: "            .. tostring(newMaterial:getClampType()))
print("Flags: "                 .. tostring(newMaterial:getMaterialFlags()))
print("Surface Flags: "         .. tostring(newMaterial:getSurfaceFlags()))
print("Deform Type: "           .. tostring(newMaterial:getDeformType()))
print("Deform Expression #1: "  .. tostring(newMaterial:getDeformExpressionString(0)))
print("Deform Decl Name: "      .. tostring(newMaterial:getDeformDeclName()))
print("Spectrum: "              .. tostring(newMaterial:getSpectrum()))
print("DecalInfo.stay: "        .. tostring(newMaterial:getDecalInfo().stayMilliSeconds))
print("DecalInfo.fade: "        .. tostring(newMaterial:getDecalInfo().fadeMilliSeconds))
print("DecalInfo.startColour: " .. tostring(newMaterial:getDecalInfo().startColour))
print("DecalInfo.endColour: "   .. tostring(newMaterial:getDecalInfo().endColour))
print("Coverage: "              .. tostring(newMaterial:getCoverage()))
print("LightFalloffCubeMap: "   .. tostring(newMaterial:getLightFalloffCubeMapType()))
print("RenderBumpArgs: "        .. tostring(newMaterial:getRenderBumpArguments()))
print("FrobStage Type: "        .. tostring(newMaterial:getFrobStageType()))

newMaterial:setSortRequest(Material.SortRequest.NEAREST)
newMaterial:setClampType(Material.ClampType.NOREPEAT)
newMaterial:setCullType(Material.CullType.NONE)
newMaterial:setPolygonOffset(0.3)
newMaterial:setMaterialFlag(Material.Flag.NOSHADOWS)
newMaterial:setSurfaceFlag(Material.SurfaceFlag.LADDER)
newMaterial:setSurfaceFlag(Material.SurfaceFlag.NONSOLID)
newMaterial:setSurfaceType(Material.SurfaceType.WOOD)
newMaterial:setSpectrum(5)
newMaterial:setIsFogLight(true)
newMaterial:setIsBlendLight(false)
newMaterial:setDescription("New Material")

newMaterial:setFrobStageType(Material.FrobStageType.DIFFUSE)
newMaterial:setFrobStageParameter(0, 0.4)
newMaterial:setFrobStageRgbParameter(1, 0.1, 0.2, 0.3)

print(string.format("\nMaterial definition with frobstage_diffuse:\n%s\n{%s}\n",
    newMaterial:getName(), newMaterial:getDefinition()))

newMaterial:setFrobStageType(Material.FrobStageType.TEXTURE)
newMaterial:setFrobStageMapExpressionFromString("textures/common/white")

print(string.format("\nMaterial definition with frobstage_texture:\n%s\n{%s}\n",
    newMaterial:getName(), newMaterial:getDefinition()))

local stageIndex = newMaterial:addStage(MaterialStage.Type.BLEND)
print("Material has now " .. tostring(newMaterial:getNumStages()) .. " stages")
newMaterial:removeStage(stageIndex)

local diffuseStageIndex = newMaterial:addStage(MaterialStage.Type.DIFFUSE)
local bumpStageIndex    = newMaterial:duplicateStage(diffuseStageIndex)
print("Material has now " .. tostring(newMaterial:getNumStages()) .. " stages")

-- Edit diffuse stage
local editD = newMaterial:getEditableStage(diffuseStageIndex)
editD:setStageFlag(MaterialStage.Flag.IGNORE_ALPHATEST)
editD:clearStageFlag(MaterialStage.Flag.FILTER_LINEAR)
editD:setMapType(MaterialStage.MapType.CUBEMAP)
editD:setMapExpressionFromString("env/sky1")
editD:setBlendFuncStrings({"gl_one", "gl_dest_alpha"})
editD:setAlphaTestExpressionFromString("sinTable[time]")
editD:addTransformation(MaterialStage.TransformType.SCALE, "time", "global0")
local rotateIdx = editD:addTransformation(MaterialStage.TransformType.ROTATE, "time*0.5", "")
editD:updateTransformation(rotateIdx, MaterialStage.TransformType.SCALE, "time*0.5", "0.5")
editD:removeTransformation(rotateIdx)
editD:setColourExpressionFromString(MaterialStage.ColourComponent.RGB, "0.4*time")
editD:setConditionExpressionFromString("parm4 > 7")
editD:setTexGenType(MaterialStage.TexGenType.REFLECT)
editD:setTexGenType(MaterialStage.TexGenType.WOBBLESKY)
editD:setTexGenExpressionFromString(0, "0.1")
editD:setTexGenExpressionFromString(1, "0.2")
editD:setTexGenExpressionFromString(2, "0.3")
editD:setVertexColourMode(MaterialStage.VertexColourMode.MULTIPLY)
editD:setClampType(Material.ClampType.NOREPEAT)
editD:setPrivatePolygonOffset(-1.2)
editD:setRenderMapSize(640, 480)

-- Edit bump stage
local editB = newMaterial:getEditableStage(bumpStageIndex)
editB:setSoundMapWaveForm(true)
editB:setMapType(MaterialStage.MapType.VIDEOMAP)
editB:setVideoMapProperties("videos/blah", true)

-- Print all stages
for _, stage in ipairs(newMaterial:getAllStages()) do
    print("Stage type: "              .. tostring(stage:getType()))
    print("Stage map type: "          .. tostring(stage:getMapType()))
    print("Stage map expression: "    .. tostring(stage:getMapExpressionString()))
    print("Stage flags: "             .. tostring(stage:getStageFlags()))
    print("Stage clamp type: "        .. tostring(stage:getClampType()))
    print("Stage texgen type: "       .. tostring(stage:getTexGenType()))
    print("Stage texgen expr #1: "    .. tostring(stage:getTexGenExpressionString(0)))
    local bf = stage:getBlendFuncStrings()
    print("Stage blend funcs: "       .. bf[1] .. "," .. bf[2])
    print("Stage colour RGB: "        .. tostring(stage:getColourExpressionString(MaterialStage.ColourComponent.RGB)))
    print("Stage alpha test: "        .. tostring(stage:getAlphaTestExpressionString()))
    print("Stage condition: "         .. tostring(stage:getConditionExpressionString()))
    print("Stage vertex program: "    .. tostring(stage:getVertexProgram()))
    print("Stage fragment program: "  .. tostring(stage:getFragmentProgram()))
    print(string.format("Stage has %d vertex parms and %d fragment maps",
        stage:getNumVertexParms(), stage:getNumFragmentMaps()))
    print("Stage private poly offset: " .. tostring(stage:getPrivatePolygonOffset()))

    for _, transform in ipairs(stage:getTransformations()) do
        print("Transform type: "  .. tostring(transform.type))
        print("Transform expr1: " .. tostring(transform.expression1))
        print("Transform expr2: " .. tostring(transform.expression2))
    end
    for v = 0, stage:getNumVertexParms() - 1 do
        local parm = stage:getVertexParm(v)
        print(string.format("Vertex Parm #%d: %s", parm.index,
            table.concat(parm.expressions, " ")))
    end
    for fmi = 0, stage:getNumFragmentMaps() - 1 do
        local fm = stage:getFragmentMap(fmi)
        print(string.format("Fragment Map #%d: %s %s",
            fm.index, table.concat(fm.options, " "), fm.mapExpression))
    end
end

newMaterial:swapStagePosition(diffuseStageIndex, bumpStageIndex)

print(newMaterial:isModified() and "The new material has been modified"
                               or  "The new material is not modified?")

print(string.format("\n\nFull Material definition:\n%s\n{%s}\n",
    newMaterial:getName(), newMaterial:getDefinition()))

GlobalMaterialManager:saveMaterial(newMaterial:getName())

print(not newMaterial:isModified() and "After save, the material is not modified"
                                   or  "After save, the material is still modified?")

newMaterial:clearMaterialFlag(Material.Flag.NOSHADOWS)
newMaterial:clearSurfaceFlag(Material.SurfaceFlag.NONSOLID)
newMaterial:setSurfaceType(Material.SurfaceType.DEFAULT)

local fullPath = GlobalFileSystem:findFile(filename) .. filename
print(fullPath)
os.remove(fullPath)

print("Removing " .. newMaterial:getName() .. " again...")
GlobalMaterialManager:removeMaterial(newMaterial:getName())

print("--- Done ---")
