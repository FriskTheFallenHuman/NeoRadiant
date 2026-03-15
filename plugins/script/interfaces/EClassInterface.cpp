#include "EClassInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_ECLASS = "NeoRadiant.EntityClass";

void EClassInterface::registerInterface( lua_State* L )
{
	static luaL_Reg eclassMethods[] =
		{ { "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS )->getDeclName() );
				return 1;
			} },
		{ "isOfType",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS )->isOfType( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "getColour",
			[](lua_State* L)->int {
				auto c				= lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS )->getColour();
				lua_newtable( L );
				lua_pushnumber( L, c.x() );
				lua_rawseti( L, -2, 1 );
				lua_pushnumber( L, c.y() );
				lua_rawseti( L, -2, 2 );
				lua_pushnumber( L, c.z() );
				lua_rawseti( L, -2, 3 );
				return 1;
			} },
		{ "isFixedSize",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS )->isFixedSize() );
				return 1;
			} },
		{ "getAttribute",
			[](lua_State* L)->int {
				auto* ec = lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS );
				const std::string key				= lua_checkstdstring( L, 2 );
				const EntityClassAttribute* attr			= ec->getAttribute( key );
				if( attr ) {
					lua_pushstdstring( L, attr->getValue() );
					return 1;
				}
				// Try parent
				auto parentPtr = ec->getParent();
				while( parentPtr ) {
					attr = parentPtr->getAttribute( key );
					if( attr ) {
						lua_pushstdstring( L, attr->getValue() );
						return 1;
					}
					parentPtr = parentPtr->getParent();
				}
				lua_pushstring( L, "" );
				return 1;
			} },
		{ "forEachAttribute",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref				= luaL_ref( L, LUA_REGISTRYINDEX );
				lua_checkobject<scene::EntityClass>( L, 1, META_ECLASS )->forEachAttribute( [&]( const EntityClassAttribute& a, bool ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushstdstring( L, a.getName() );
					lua_pushstdstring( L, a.getValue() );
					lua_pushstdstring( L, a.getType() );
					if( lua_pcall( L, 3, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_ECLASS, eclassMethods );

	static luaL_Reg mgr[] =
		{ { "findClass",
			[](lua_State* L)->int {
				auto ec = GlobalEntityClassManager().findClass( lua_checkstdstring( L, 2 ) );
				if( !ec ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, ec.get(), META_ECLASS );
				return 1;
			} },
		{ "forEachEntityClass",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalEntityClassManager().forEachEntityClass( [&]( const scene::EntityClass::Ptr& ec ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, ec.get(), META_ECLASS );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.EntityClassManager", mgr );

	lua_setglobal_object( L, "GlobalEntityClassManager", this, "NeoRadiant.EntityClassManager" );
}

} // namespace script
