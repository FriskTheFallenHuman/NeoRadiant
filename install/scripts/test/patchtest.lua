-- patchtest.lua
-- Demonstrates patch traversal

GlobalSceneGraph:root():traverse(function(node)
    if node:isPatch() then
        local patch = node:getPatch()
        print("Patch information:")
        print("Dimensions: " .. tostring(patch:getWidth()) .. "x" .. tostring(patch:getHeight()))

        local mesh = patch:getTesselatedPatchMesh()
        print("Mesh dimension: w=" .. tostring(mesh.width) .. ", h=" .. tostring(mesh.height))

        for _, v in ipairs(mesh.vertices) do
            print("Mesh vertex: " .. tostring(v.vertex:x()) .. ","
                                   .. tostring(v.vertex:y()) .. ","
                                   .. tostring(v.vertex:z()))
        end
    end
    return true
end)
