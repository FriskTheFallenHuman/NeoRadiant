#include "MapInterface.h"

#include "../LuaHelper.h"

#include "imap.h"

namespace script
{

void MapInterface::registerInterface( lua_State* L )
{
	// MapEditMode constants
	lua_pushinteger( L, ( lua_Integer )IMap::EditMode::Normal );
	lua_setglobal( L, "MAP_MODE_NORMAL" );
	lua_pushinteger( L, ( lua_Integer )IMap::EditMode::Merge );
	lua_setglobal( L, "MAP_MODE_MERGE" );

	static const luaL_Reg methods[] =
		{ { "getWorldSpawn",
			[](lua_State* L)->int {
				lua_pushscenenode( L, GlobalMapModule().getWorldspawn() );
				return 1;
			} },
		{ "getMapName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalMapModule().getMapName() );
				return 1;
			} },
		{ "getRoot",
			[](lua_State* L)->int {
				lua_pushscenenode( L, GlobalMapModule().getRoot() );
				return 1;
			} },
		{ "isModified",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalMapModule().isModified() );
				return 1;
			} },
		{ "getEditMode",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )GlobalMapModule().getEditMode() );
				return 1;
			} },
		{ "setEditMode",
			[](lua_State* L)->int {
				GlobalMapModule().setEditMode( static_cast<IMap::EditMode>( luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "showPointFile",
			[](lua_State* L)->int {
				std::string p			= lua_checkstdstring( L, 2 );
				if( !p.empty() )
					GlobalMapModule().showPointFile( p );
				return 0;
			} },
		{ "isPointTraceVisible",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalMapModule().isPointTraceVisible() );
				return 1;
			} },
		{ "getPointFileList",
			[](lua_State* L)->int {
				lua_newtable( L );
				int		  i			= 1;
				GlobalMapModule().forEachPointfile( [&]( const fs::path& p ) {
					lua_pushstdstring( L, p.string() );
					lua_rawseti( L, -2, i++ );
				} );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.Map", methods );

	lua_setglobal_object( L, "GlobalMap", this, "NeoRadiant.Map" );
}

} // namespace script
