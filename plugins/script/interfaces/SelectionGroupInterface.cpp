#include "SelectionGroupInterface.h"
#include "../LuaHelper.h"
#include "SceneGraphInterface.h"
#include "iselectiongroup.h"

namespace script
{

constexpr const char* META_SELGROUP = "NeoRadiant.SelectionGroup";

static selection::ISelectionGroupManager& getGroupMgr( lua_State* L )
{
	auto root = GlobalMapModule().getRoot();
	if( !root )
		luaL_error( L, "No map is currently loaded" );
	return root->getSelectionGroupManager();
}

void SelectionGroupInterface::registerInterface( lua_State* L )
{
	static luaL_Reg groupMethods[] =
		{ { "getId",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->getId() );
				return 1;
			} },
		{ "setName",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->setName( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->getName() );
				return 1;
			} },
		{ "addNode",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node )
					lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->addNode( ud->node );
				return 0;
			} },
		{ "removeNode",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node )
					lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->removeNode( ud->node );
				return 0;
			} },
		{ "size",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->size() );
				return 1;
			} },
		{ "setSelected",
			[](lua_State* L)->int {
				lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->setSelected( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "foreachNode",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref = luaL_ref( L, LUA_REGISTRYINDEX );
				lua_checkobject<selection::ISelectionGroup>( L, 1, META_SELGROUP )->foreachNode( [&]( const scene::INodePtr& n ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushscenenode( L, n );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_SELGROUP, groupMethods );

	static luaL_Reg mgr[] =
		{ { "createSelectionGroup",
			[](lua_State* L)->int {
				auto g = getGroupMgr( L ).createSelectionGroup();
				if( !g ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, g.get(), META_SELGROUP );
				return 1;
			} },
		{ "deleteSelectionGroup",
			[](lua_State* L)->int {
				getGroupMgr( L ).deleteSelectionGroup( ( std::size_t )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "deleteAllSelectionGroups",
			[](lua_State* L)->int {
				getGroupMgr( L ).deleteAllSelectionGroups();
				return 0;
			} },
		{ "getSelectionGroup",
			[](lua_State* L)->int {
				auto g	  = getGroupMgr( L ).getSelectionGroup( ( std::size_t )luaL_checkinteger( L, 2 ) );
				if( !g ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, g.get(), META_SELGROUP );
				return 1;
			} },
		{ "foreachSelectionGroup",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				getGroupMgr( L ).foreachSelectionGroup( [&]( selection::ISelectionGroup& g ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, &g, META_SELGROUP );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.SelectionGroupManager", mgr );
	lua_setglobal_object( L, "GlobalSelectionGroupManager", this, "NeoRadiant.SelectionGroupManager" );
}

} // namespace script
