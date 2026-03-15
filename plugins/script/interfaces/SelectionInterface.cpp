#include "SelectionInterface.h"
#include "../LuaHelper.h"

namespace script
{

void SelectionInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "getSelectionInfo",
			[](lua_State* L)->int {
				const SelectionInfo& si = GlobalSelectionSystem().getSelectionInfo();
				lua_newtable( L );
				lua_pushinteger( L, si.totalCount );
				lua_setfield( L, -2, "totalCount" );
				lua_pushinteger( L, si.brushCount );
				lua_setfield( L, -2, "brushCount" );
				lua_pushinteger( L, si.patchCount );
				lua_setfield( L, -2, "patchCount" );
				lua_pushinteger( L, si.entityCount );
				lua_setfield( L, -2, "entityCount" );
				lua_pushinteger( L, si.componentCount );
				lua_setfield( L, -2, "componentCount" );
				return 1;
			} },
		{ "foreachSelected",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int		  ref		= luaL_ref( L, LUA_REGISTRYINDEX );
				struct V : selection::SelectionSystem::Visitor {
					lua_State* L;
					int		   ref;
					void	   visit( const scene::INodePtr& n ) const override
					{
						lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
						lua_pushscenenode( L, n );
						if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
							lua_pop( L, 1 );
					}
				};
				V		  v;
				v.L					= L;
				v.ref				= ref;
				GlobalSelectionSystem().foreachSelected( v );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "foreachSelectedComponent",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int		  ref		= luaL_ref( L, LUA_REGISTRYINDEX );
				struct V : selection::SelectionSystem::Visitor {
					lua_State* L;
					int		   ref;
					void	   visit( const scene::INodePtr& n ) const override
					{
						lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
						lua_pushscenenode( L, n );
						if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
							lua_pop( L, 1 );
					}
				};
				V		  v;
				v.L					= L;
				v.ref				= ref;
				GlobalSelectionSystem().foreachSelectedComponent( v );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "foreachSelectedFace",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int		  ref		= luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalSelectionSystem().foreachFace( [&]( IFace& face ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushface( L, &face );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "setSelectedAll",
			[](lua_State* L)->int {
				GlobalSelectionSystem().setSelectedAll( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "setSelectedAllComponents",
			[](lua_State* L)->int {
				GlobalSelectionSystem().setSelectedAllComponents( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "ultimateSelected",
			[](lua_State* L)->int {
				lua_pushscenenode( L, GlobalSelectionSystem().ultimateSelected() );
				return 1;
			} },
		{ "penultimateSelected",
			[](lua_State* L)->int {
				lua_pushscenenode( L, GlobalSelectionSystem().penultimateSelected() );
				return 1;
			} },

		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.SelectionSystem", methods );

	lua_setglobal_object( L, "GlobalSelectionSystem", this, "NeoRadiant.SelectionSystem" );
}

} // namespace script
