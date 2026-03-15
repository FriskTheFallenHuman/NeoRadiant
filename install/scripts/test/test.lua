-- test.lua
-- Comprehensive NeoRadiant Lua scripting API test

-- Test the Registry interface
local value = GlobalRegistry:get("user/paths/appPath")
print(value)

local worldspawn = Radiant:findEntityByClassname("worldspawn")
worldspawn:setKeyValue("test", "success")
print("Worldspawn edited")

worldspawn = Radiant:findEntityByName("world")
worldspawn:setKeyValue("test", "another success")
print("Worldspawn edited")

-- Test the DeclarationManager interface
GlobalDeclarationManager:foreachDeclaration(Declaration.Type.Skin, function(decl)
    print(tostring(decl:getDeclType()) .. ": " .. decl:getDeclName())
end)

local caulk = GlobalDeclarationManager:findDeclaration(
    Declaration.Type.Material, "textures/common/caulk")
print("Name: "       .. caulk:getDeclName())
print("Type: "       .. tostring(caulk:getDeclType()))
print("Defined in: " .. tostring(caulk:getDeclFilePath()))
print("Definition: " .. caulk:getDeclSource().contents)

GlobalDeclarationManager:foreachDeclaration(Declaration.Type.ModelDef, function(decl)
    print(tostring(decl:getDeclType()) .. ": " .. decl:getDeclName())
end)

-- FX interface
local fx = GlobalFxManager:findFx("fx/sparks")
if not fx:isNull() then
    print("Name: "        .. fx:getDeclName())
    print("Type: "        .. tostring(fx:getDeclType()))
    print("Defined in: "  .. tostring(fx:getDeclFilePath()))
    print("Definition: "  .. fx:getDeclSource().contents)
    print("Num actions: " .. tostring(fx:getNumActions()))

    local action = fx:getAction(1)
    print("Action #1 type: "             .. tostring(action:getActionType()))
    print("Action #1 name: "             .. tostring(action:getName()))
    print("Action #1 delay: "            .. tostring(action:getDelayInSeconds()))
    print("Action #1 duration: "         .. tostring(action:getDurationInSeconds()))
    print("Action #1 ignoreMaster: "     .. tostring(action:getIgnoreMaster()))
    print("Action #1 shakeTime: "        .. tostring(action:getShakeTimeInSeconds()))
    print("Action #1 shakeAmplitude: "   .. tostring(action:getShakeAmplitude()))
    print("Action #1 shakeDistance: "    .. tostring(action:getShakeDistance()))
    print("Action #1 shakeFalloff: "     .. tostring(action:getShakeFalloff()))
    print("Action #1 shakeImpulse: "     .. tostring(action:getShakeImpulse()))
    print("Action #1 noShadows: "        .. tostring(action:getNoShadows()))
    print("Action #1 fireSibling: "      .. tostring(action:getFireSiblingAction()))
    print("Action #1 randomDelay: "      .. tostring(action:getRandomDelay()))
    print("Action #1 rotate: "           .. tostring(action:getRotate()))
    print("Action #1 trackOrigin: "      .. tostring(action:getTrackOrigin()))
    print("Action #1 restart: "          .. tostring(action:getRestart()))
    print("Action #1 fadeIn: "           .. tostring(action:getFadeInTimeInSeconds()))
    print("Action #1 fadeOut: "          .. tostring(action:getFadeOutTimeInSeconds()))
    print("Action #1 decalSize: "        .. tostring(action:getDecalSize()))
    print("Action #1 offset: "           .. tostring(action:getOffset()))
    print("Action #1 axis: "             .. tostring(action:getAxis()))
    print("Action #1 angle: "            .. tostring(action:getAngle()))
    print("Action #1 useLight: "         .. tostring(action:getUseLight()))
    print("Action #1 useModel: "         .. tostring(action:getUseModel()))
    print("Action #1 attachLight: "      .. tostring(action:getAttachLight()))
    print("Action #1 attachEntity: "     .. tostring(action:getAttachEntity()))
    print("Action #1 launchProjectile: " .. tostring(action:getLaunchProjectileDef()))
    print("Action #1 lightMaterial: "    .. tostring(action:getLightMaterialName()))
    print("Action #1 lightColour: "      .. tostring(action:getLightRgbColour()))
    print("Action #1 lightRadius: "      .. tostring(action:getLightRadius()))
    print("Action #1 modelName: "        .. tostring(action:getModelName()))
    print("Action #1 decalMaterial: "    .. tostring(action:getDecalMaterialName()))
    print("Action #1 particleTrack: "    .. tostring(action:getParticleTrackVelocity()))
    print("Action #1 soundShader: "      .. tostring(action:getSoundShaderName()))
    print("Action #1 shockwave: "        .. tostring(action:getShockwaveDefName()))
end

-- Create a new material declaration
local myOwnMaterial = GlobalDeclarationManager:findOrCreateDeclaration(
    Declaration.Type.Material, "textures/myown_material")
local syntax = myOwnMaterial:getDeclSource()
syntax.contents = "diffusemap _white"
myOwnMaterial:setDeclSource(syntax)
myOwnMaterial:setDeclFilePath("materials/", "script_test.mtr")
GlobalDeclarationManager:saveDeclaration(myOwnMaterial)

-- Test the EClassManager interface
local eclass = GlobalEntityClassManager:findClass("atdm:func_shooter")
print(eclass:getAttribute("editor_usage"):getValue())

if not eclass:isNull() then
    local shooter = GlobalEntityCreator:createEntity(eclass)
end

local modelDef = GlobalEntityClassManager:findModel("tdm_ai_citywatch")
print("ModelDef mesh for tdm_ai_citywatch = " .. modelDef.mesh)

for anim, animFile in pairs(modelDef.anims) do
    print(anim .. " = " .. animFile)
end

-- Traverse the scenegraph
GlobalSceneGraph:root():traverse(function(node)
    print(node:getNodeType())
    local model = node:getModel()
    if not model:isNull() then
        print("Node is a model")
    else
        print("Node is not a model")
    end
    return true
end)

-- Traverse the current selection
GlobalSelectionSystem:foreachSelected(function(node)
    local brush = node:getBrush()
    if not brush:isNull() then
        print(tostring(brush:getNumFaces()))
    else
        print("Node is not a brush")
    end

    local patch = node:getPatch()
    if not patch:isNull() then
        print("Node is a patch")
    else
        print("Node is not a patch")
    end

    local model = node:getModel()
    if not model:isNull() then
        print("Node is a model")
    else
        print("Node is not a model")
    end
end)

-- Visit every selected face
GlobalSelectionSystem:foreachSelectedFace(function(face)
    print(face:getShader())
end)

print("Map name is " .. GlobalMap:getMapName())
print(tostring(GlobalMap:getEditMode()))

-- Switching Map Modes
GlobalMap:setEditMode(MapEditMode.Merge)
GlobalMap:setEditMode(MapEditMode.Normal)

-- Point File Management
print(tostring(GlobalMap:showPointFile("test.lin")))
print(tostring(GlobalMap:isPointTraceVisible()))

local ptFiles = GlobalMap:getPointFileList()
print("Point files found: " .. tostring(#ptFiles))
for _, path in ipairs(ptFiles) do
    print("Pointfile: " .. path)
end

-- Find map's worldspawn
local wsNode = GlobalMap:getWorldSpawn()
if not wsNode:isNull() then
    local wsEnt = wsNode:getEntity()
    if not wsEnt:isNull() then
        print("Spawnclass of worldspawn: " .. wsEnt:getKeyValue("spawnclass"))
    end
else
    print("There is no worldspawn in this map yet")
end

-- Test EntityVisitor
if not wsNode:isNull() then
    local wsEnt = wsNode:getEntity()
    wsEnt:forEachKeyValue(function(key, val)
        print("Worldspawn has spawnarg: " .. key .. " = " .. val)
    end)

    local keyvalues = wsEnt:getKeyValuePairs("t")
    for _, kv in ipairs(keyvalues) do
        print("Keyvalue " .. kv[1] .. " = " .. kv[2])
    end
end

-- Test the command system
GlobalCommandSystem:execute('texscale "0 0.1"')

-- Test the GameManager interface
print("Mod path = " .. GlobalGameManager:getModPath())
local game = GlobalGameManager:currentGame()
print("Current game type: " .. game:getKeyValue("type"))

print("VFS Search paths:")
local vfsPaths = GlobalGameManager:getVFSSearchPaths()
for _, path in ipairs(vfsPaths) do
    print(path)
end

-- Test FileSystem (VFS)
GlobalFileSystem:forEachFile("skins/", "skin", function(filename)
    print("Found file: " .. filename)
end, 99)

local filecontents = GlobalFileSystem:readTextFile("skins/tdm_ai_guard_citywatch.skin")
print(filecontents)

-- Test the Grid Interface
print("Current grid size = " .. tostring(GlobalGrid:getGridSize()))

-- Test the ShaderSystem interface (material browser)
local material = GlobalMaterialManager:getMaterialForName("bc_rat")
if not material:isNull() then
    print("Material " .. material:getName() .. " is defined in " .. material:getShaderFileName())
end

-- Test finding a model
GlobalSceneGraph:root():traverse(function(node)
    local model = node:getModel()
    if not model:isNull() then
        print("Model information:")
        print("Filename: "      .. model:getFilename())
        print("Model path: "    .. model:getModelPath())
        print("Surface count: " .. tostring(model:getSurfaceCount()))
        print("Vertex count: "  .. tostring(model:getVertexCount()))
        print("Poly count: "    .. tostring(model:getPolyCount()))

        local materials = model:getActiveMaterials()
        print("Active Materials:")
        for _, mat in ipairs(materials) do print(mat) end

        for i = 0, model:getSurfaceCount() - 1 do
            local surface = model:getSurface(i)
            print("Surface: "       .. tostring(i))
            print("  Default: "     .. surface:getDefaultMaterial())
            print("  Active: "      .. surface:getActiveMaterial())
            print("  PolyCount: "   .. tostring(surface:getNumTriangles()))
            print("  VertexCount: " .. tostring(surface:getNumVertices()))

            local s = Vector3(0, 0, 0)
            for v = 0, surface:getNumVertices() - 1 do
                local mv = surface:getVertex(v)
                s = s + mv.vertex
            end
            print("  Sum: " .. tostring(s:x()) .. "," .. tostring(s:y()) .. "," .. tostring(s:z()))
        end
    end
    return true
end)

-- Test patch manipulation
GlobalSceneGraph:root():traverse(function(node)
    if node:isPatch() then
        local patch = node:getPatch()
        print("Patch information:")
        print("Dimensions: " .. tostring(patch:getWidth()) .. "x" .. tostring(patch:getHeight()))
        for w = 0, patch:getWidth() - 1 do
            for h = 0, patch:getHeight() - 1 do
                local ctrl = patch:ctrlAt(h, w)
                ctrl.vertex = ctrl.vertex + Vector3(0, 0, 10 * (h - w))
            end
        end
        patch:controlPointsChanged()
    end
    return true
end)

-- Test the SelectionSetManager interface
GlobalSelectionSetManager:foreachSelectionSet(function(selectionset)
    print(selectionset:getName())
end)

local selSet = GlobalSelectionSetManager:createSelectionSet("TestSelectionSet")
selSet:assignFromCurrentScene()
selSet:deselect()
selSet:select()
selSet:clear()
if selSet:empty() then
    GlobalSelectionSetManager:deleteSelectionSet("TestSelectionSet")
end
print("")

-- Test SoundManager
local soundshader = GlobalSoundManager:getSoundShader("tdm_ai_lady_alertdown_to_idle")
if not soundshader:isNull() then
    print("Sound shader: " .. soundshader:getName())
    local radii = soundshader:getRadii()
    print("Min radius (m): " .. tostring(radii:getMin(1)))
    print("Max radius (m): " .. tostring(radii:getMax(1)))
    local fileList = soundshader:getSoundFileList()
    for i, fname in ipairs(fileList) do
        print("Sound file: " .. fname)
    end
    if #fileList > 0 then
        GlobalSoundManager:playSound(fileList[1])
    end
end

-- Test SelectionGroup interface
local group = GlobalSelectionGroupManager:createSelectionGroup()
print("Created group with ID: " .. tostring(group:getId()))

GlobalSelectionSystem:foreachSelected(function(node)
    group:addNode(node)
end)
print("Group contains " .. tostring(group:size()) .. " items")

GlobalSelectionGroupManager:setGroupSelected(group:getId(), false)

group:foreachNode(function(node)
    print("Group Member: " .. node:getNodeType())
end)

-- Camera
local camview = GlobalCameraManager:getActiveView()
print(tostring(camview:getCameraOrigin()))
camview:setCameraOrigin(Vector3(50, 0, 50))

-- Layer Functionality
local function printLayers()
    GlobalLayerManager:foreachLayer(function(layerID, layerName)
        print(tostring(layerID) .. " " .. layerName)
    end)
    print("=================")
end

print("Layers:")
printLayers()

print("Test create")
print(tostring(GlobalLayerManager:createLayer("One")))
print(tostring(GlobalLayerManager:createLayer("Two")))
print(tostring(GlobalLayerManager:createLayer("Forty-two", 42)))
print(tostring(GlobalLayerManager:createLayer("TwoAgain", 2)))
printLayers()

print("Test delete")
print(tostring(GlobalLayerManager:deleteLayer("NotALayer")))
print(tostring(GlobalLayerManager:deleteLayer("TwoAgain")))
printLayers()

print("Test get")
print(tostring(GlobalLayerManager:getLayerID("Forty-two")))
print(tostring(GlobalLayerManager:getLayerName(42)))

print("Test exists")
print(tostring(GlobalLayerManager:layerExists(123)))
print(tostring(GlobalLayerManager:layerExists(42)))

print("Test rename")
print(tostring(GlobalLayerManager:renameLayer(42, "Forty-two")))
print(tostring(GlobalLayerManager:renameLayer(42, "Two")))
print(tostring(GlobalLayerManager:renameLayer(42, "HHGTTG")))

print("Test active")
GlobalLayerManager:setActiveLayer(3)
print(tostring(GlobalLayerManager:getActiveLayer()))
GlobalLayerManager:setActiveLayer(2)
print(tostring(GlobalLayerManager:getActiveLayer()))

print("Test visible")
GlobalLayerManager:setLayerVisibility("One", false)
print(tostring(GlobalLayerManager:layerIsVisible("One")))
GlobalLayerManager:setLayerVisibility("One", true)
print(tostring(GlobalLayerManager:layerIsVisible("One")))

GlobalLayerManager:setLayerVisibility(1, false)
print(tostring(GlobalLayerManager:layerIsVisible(1)))
GlobalLayerManager:setLayerVisibility(1, true)
print(tostring(GlobalLayerManager:layerIsVisible(1)))

GlobalLayerManager:setSelected(0, true)
GlobalLayerManager:moveSelectionToLayer(1)
