#include "GameInterface.h"
#include "../LuaHelper.h"

namespace script
{

void GameInterface::registerInterface( lua_State* L )
{
	// Game object
	static const luaL_Reg gameMethods[] =
		{ { "getKeyValue",
			[](lua_State* L)->int {
				auto* g = lua_checkobject<game::IGame>( L, 1, "NeoRadiant.Game" );
				lua_pushstdstring( L, g->getKeyValue( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.Game", gameMethods );

	// GameManager
	static const luaL_Reg methods[] =
		{ { "getUserEnginePath",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalGameManager().getUserEnginePath() );
				return 1;
			} },
		{ "getModPath",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalGameManager().getModPath() );
				return 1;
			} },
		{ "getModBasePath",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalGameManager().getModBasePath() );
				return 1;
			} },
		{ "currentGame",
			[](lua_State* L)->int {
				auto	  g			= GlobalGameManager().currentGame();
				if( !g ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, g.get(), "NeoRadiant.Game" );
				return 1;
			} },
		{ "getVFSSearchPaths",
			[](lua_State* L)->int {
				auto	  paths		= GlobalGameManager().getVFSSearchPaths();
				lua_newtable( L );
				int		  i			= 1;
				for( const auto& p : paths ) {
					lua_pushstdstring( L, p );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.GameManager", methods );

	lua_setglobal_object( L, "GlobalGameManager", this, "NeoRadiant.GameManager" );
}

} // namespace script
