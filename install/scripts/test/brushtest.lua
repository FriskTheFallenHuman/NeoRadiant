-- brushtest.lua
-- Lua equivalent of the old brushtest.py
-- Demonstrates the DR Lua scripting API

GlobalSceneGraph:root():traverse(function(node)
    -- node:isBrush() is added to every SceneNode by BrushInterface
    if node:isBrush() then
        print("Brush information:")
        print("Number of faces: " .. tostring(node:getNumFaces()))

        for i = 1, node:getNumFaces() do
            local face = node:getFace(i)           -- 1-based in Lua
            print("Face #" .. tostring(i) .. " has shader " .. face:getShader())

            local winding = face:getWinding()       -- table of vertex tables
            for _, v in ipairs(winding) do
                print("  w.vertex=" .. tostring(v.vertex:x()) .. ","
                                     .. tostring(v.vertex:y()) .. ","
                                     .. tostring(v.vertex:z()))
            end

            face:fitTexture(1, 1)
        end
    end

    return true   -- continue traversal
end)

print("")
