#include "AasFileInterface.h"

#include "../LuaHelper.h"
#include "MathInterface.h"

#include "iaasfile.h"
#include "imap.h"
#include "iarchive.h"
#include "ifilesystem.h"
#include "itextstream.h"

namespace script
{

constexpr const char* META_AASFILE = "NeoRadiant.AasFile";

// Keeps the shared_ptr alive for as long as Lua holds the userdata.
struct AasFileUD
{
    map::IAasFilePtr ptr;
};

static void lua_pushaasfile(lua_State* L, const map::IAasFilePtr& file)
{
    if (!file) {
        lua_pushnil(L);
        return;
    }
    auto* ud = static_cast<AasFileUD*>(lua_newuserdata(L, sizeof(AasFileUD)));
    new(ud) AasFileUD{ file };
    luaL_setmetatable(L, META_AASFILE);
}

static void push_AasType(lua_State* L, const map::AasType& t)
{
    lua_newtable(L);
    lua_pushstdstring(L, t.entityDefName);
    lua_setfield(L, -2, "entityDefName");
    lua_pushstdstring(L, t.fileExtension);
    lua_setfield(L, -2, "fileExtension");
}

static void push_AasFileInfo(lua_State* L, const map::AasFileInfo& info)
{
    lua_newtable(L);
    lua_pushstdstring(L, info.absolutePath);
    lua_setfield(L, -2, "absolutePath");
    push_AasType(L, info.type);
    lua_setfield(L, -2, "type");
}

static void push_AasArea(lua_State* L, const map::IAasFile::Area& a)
{
    lua_newtable(L);

    lua_pushinteger(L, (lua_Integer)a.numFaces);
    lua_setfield(L, -2, "numFaces");

    lua_pushinteger(L, (lua_Integer)a.firstFace);
    lua_setfield(L, -2, "firstFace");

    lua_pushaabb(L, a.bounds);
    lua_setfield(L, -2, "bounds");

    lua_pushvec3(L, a.center);
    lua_setfield(L, -2, "center");

    lua_pushinteger(L, (lua_Integer)a.flags);
    lua_setfield(L, -2, "flags");

    lua_pushinteger(L, (lua_Integer)a.contents);
    lua_setfield(L, -2, "contents");

    lua_pushinteger(L, (lua_Integer)a.cluster);
    lua_setfield(L, -2, "cluster");

    lua_pushinteger(L, (lua_Integer)a.travelFlags);
    lua_setfield(L, -2, "travelFlags");
}

void AasFileInterface::registerInterface(lua_State* L)
{
    // ---- IAasFile metatable ----
    static const luaL_Reg aasFileMethods[] = {
        { "getNumAreas",
            [](lua_State* L)->int {
                auto* ud = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                lua_pushinteger(L, (lua_Integer)ud->ptr->getNumAreas());
                return 1;
            } },
        { "getArea",
            [](lua_State* L)->int {
                auto* ud  = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                auto  idx = (std::size_t)luaL_checkinteger(L, 2) - 1; // Lua 1-based
                push_AasArea(L, ud->ptr->getArea(idx));
                return 1;
            } },
        { "getNumVertices",
            [](lua_State* L)->int {
                auto* ud = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                lua_pushinteger(L, (lua_Integer)ud->ptr->getNumVertices());
                return 1;
            } },
        { "getVertex",
            [](lua_State* L)->int {
                auto* ud  = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                auto  idx = (std::size_t)luaL_checkinteger(L, 2) - 1;
                lua_pushvec3(L, ud->ptr->getVertex(idx));
                return 1;
            } },
        { "getNumEdges",
            [](lua_State* L)->int {
                auto* ud = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                lua_pushinteger(L, (lua_Integer)ud->ptr->getNumEdges());
                return 1;
            } },
        { "getNumFaces",
            [](lua_State* L)->int {
                auto* ud = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                lua_pushinteger(L, (lua_Integer)ud->ptr->getNumFaces());
                return 1;
            } },
        { "getNumPlanes",
            [](lua_State* L)->int {
                auto* ud = static_cast<AasFileUD*>(luaL_checkudata(L, 1, META_AASFILE));
                lua_pushinteger(L, (lua_Integer)ud->ptr->getNumPlanes());
                return 1;
            } },
        { nullptr, nullptr }
    };

    // Pass a GC function so the shared_ptr inside AasFileUD is properly released.
    lua_registerclass(L, META_AASFILE, aasFileMethods,
        [](lua_State* L)->int {
            auto* ud = static_cast<AasFileUD*>(lua_touserdata(L, 1));
            if (ud) ud->~AasFileUD();
            return 0;
        });

    // ---- AasFileManager metatable
    static const luaL_Reg mgrMethods[] = {
        { "getAasFilesForCurrentMap",
            [](lua_State* L)->int {
                auto fileList = GlobalAasFileManager().getAasFilesForMap(
                    GlobalMapModule().getMapName());

                lua_newtable(L);
                int i = 1;
                for (const auto& info : fileList) {
                    push_AasFileInfo(L, info);
                    lua_rawseti(L, -2, i++);
                }
                return 1;
            } },
        { "loadAasFile",
            [](lua_State* L)->int {
                std::string absPath = lua_checkstdstring(L, 2);

                ArchiveTextFilePtr file =
                    GlobalFileSystem().openTextFileInAbsolutePath(absPath);

                if (!file) {
                    rWarning() << "[AAS] Could not open file: " << absPath << std::endl;
                    lua_pushnil(L);
                    return 1;
                }

                std::istream stream(&file->getInputStream());
                auto loader = GlobalAasFileManager().getLoaderForStream(stream);

                if (!loader || !loader->canLoad(stream)) {
                    rWarning() << "[AAS] No suitable loader for: " << absPath << std::endl;
                    lua_pushnil(L);
                    return 1;
                }

                stream.seekg(0, std::ios_base::beg);
                lua_pushaasfile(L, loader->loadFromStream(stream));
                return 1;
            } },
        { nullptr, nullptr }
    };

    lua_registerclass(L, "NeoRadiant.AasFileManager", mgrMethods);
    lua_setglobal_object(L, "GlobalAasFileManager", this, "NeoRadiant.AasFileManager");
}

} // namespace script