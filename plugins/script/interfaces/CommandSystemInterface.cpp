#include "CommandSystemInterface.h"
#include "../LuaHelper.h"
#include "icommandsystem.h"

namespace script
{

void CommandSystemInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "execute",
			[](lua_State* L)->int {
				GlobalCommandSystem().execute( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "addStatement",
			[](lua_State* L)->int {
				GlobalCommandSystem().addStatement( lua_checkstdstring( L, 2 ), lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ "removeCommand",
			[](lua_State* L)->int {
				GlobalCommandSystem().removeCommand( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.CommandSystem", methods );
	lua_setglobal_object( L, "GlobalCommandSystem", this, "NeoRadiant.CommandSystem" );
}

} // namespace script
