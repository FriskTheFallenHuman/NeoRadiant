#include "SelectionSetInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_SELSET = "NeoRadiant.SelectionSet";

static selection::ISelectionSetManager& getSetMgr( lua_State* L )
{
	auto root = GlobalMapModule().getRoot();
	if( !root )
		luaL_error( L, "No map is currently loaded" );
	return root->getSelectionSetManager();
}

void SelectionSetInterface::registerInterface( lua_State* L )
{
	static luaL_Reg setMethods[] =
		{ { "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->getName() );
				return 1;
			} },
		{ "isEmpty",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->empty() );
				return 1;
			} },
		{ "select",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->select();
				return 0;
			} },
		{ "deselect",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->deselect();
				return 0;
			} },
		{ "clear",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->clear();
				return 0;
			} },
		{ "assignFromCurrentScene",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->assignFromCurrentScene();
				return 0;
			} },
		{ "addNode",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node )
					lua_checkobject<selection::ISelectionSet>( L, 1, META_SELSET )->addNode( ud->node );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_SELSET, setMethods );

	static luaL_Reg mgr[] =
		{ { "findSelectionSet",
			[](lua_State* L)->int {
				auto s = getSetMgr( L ).findSelectionSet( lua_checkstdstring( L, 2 ) );
				if( !s ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, s.get(), META_SELSET );
				return 1;
		} },
		{ "createSelectionSet",
			[](lua_State* L)->int {
				auto s	  = getSetMgr( L ).createSelectionSet( lua_checkstdstring( L, 2 ) );
				if( !s ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, s.get(), META_SELSET );
				return 1;
			} },
		{ "deleteSelectionSet",
			[](lua_State* L)->int {
				getSetMgr( L ).deleteSelectionSet( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "deleteAllSelectionSets",
			[](lua_State* L)->int {
				getSetMgr( L ).deleteAllSelectionSets();
				return 0;
			} },
		{ "foreachSelectionSet",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				getSetMgr( L ).foreachSelectionSet( [&]( const selection::ISelectionSetPtr& s ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, s.get(), META_SELSET );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.SelectionSetManager", mgr );

	lua_setglobal_object( L, "GlobalSelectionSetManager", this, "NeoRadiant.SelectionSetManager" );
}

} // namespace script
