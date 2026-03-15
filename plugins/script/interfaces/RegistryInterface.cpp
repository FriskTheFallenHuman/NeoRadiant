#include "RegistryInterface.h"
#include "../LuaHelper.h"

namespace script
{

void RegistryInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "get",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalRegistry().get( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "set",
			[](lua_State* L)->int {
				GlobalRegistry().set( lua_checkstdstring( L, 2 ), lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.Registry", methods );
	lua_setglobal_object( L, "GlobalRegistry", this, "NeoRadiant.Registry" );
}

} // namespace script
