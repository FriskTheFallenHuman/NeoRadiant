#include "GridInterface.h"

#include "../LuaHelper.h"

#include "itextstream.h"
#include "igrid.h"

namespace script
{

void GridInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "setGridSize",
			[](lua_State* L)->int {
				int s = ( int )luaL_checkinteger( L, 2 );
				if( s >= GRID_0125 && s <= GRID_256 )
					GlobalGrid().setGridSize( static_cast<GridSize>( s ) );
					return 0;
			} },
		{ "getGridSize",
			[](lua_State* L)->int {
				lua_pushnumber( L, GlobalGrid().getGridSize() );
				return 1;
			} },
		{ "getGridPower",
			[](lua_State* L)->int {
				lua_pushinteger( L, GlobalGrid().getGridPower() );
				return 1;
			} },
		{ "gridDown",
			[](lua_State* L)->int {
				GlobalGrid().gridDown();
				return 0;
			} },
		{ "gridUp",
			[](lua_State* L)->int {
				GlobalGrid().gridUp();
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.Grid", methods );
	lua_setglobal_object( L, "GlobalGrid", this, "NeoRadiant.Grid" );
}

} // namespace script
