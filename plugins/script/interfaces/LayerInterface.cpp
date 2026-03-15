#include "LayerInterface.h"
#include "../LuaHelper.h"

namespace script
{

static scene::ILayerManager* getLayerMgr()
{
	auto root = GlobalMapModule().getRoot();
	if( !root )
		return nullptr;
	return &root->getLayerManager();
}

void LayerInterface::registerInterface( lua_State* L )
{
	static luaL_Reg mgr[] = {
		{ "createLayer",
			[](lua_State* L)->int {
				auto* m = getLayerMgr();
				if( !m ) {
					lua_pushinteger( L, -1 );
					return 1;
				}
				lua_pushinteger( L, m->createLayer( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "createLayerWithId",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( !m ) {
					lua_pushinteger( L, -1 );
					return 1;
				}
				lua_pushinteger( L, m->createLayer( lua_checkstdstring( L, 2 ), ( int )luaL_checkinteger( L, 3 ) ) );
				return 1;
			} },
		{ "deleteLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->deleteLayer( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "foreachLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( !m )
					return 0;
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				m->foreachLayer( [&]( int id, const std::string& name ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushinteger( L, id );
					lua_pushstdstring( L, name );
					if( lua_pcall( L, 2, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "getLayerID",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( !m ) {
					lua_pushinteger( L, -1 );
					return 1;
				}
				lua_pushinteger( L, m->getLayerID( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "getLayerName",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( !m ) {
					lua_pushstdstring( L, "" );
					return 1;
				}
				lua_pushstdstring( L, m->getLayerName( ( int )luaL_checkinteger( L, 2 ) ) );
				return 1;
			} },
		{ "layerExists",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				lua_pushboolean( L, m && m->layerExists( ( int )luaL_checkinteger( L, 2 ) ) );
				return 1;
			} },
		{ "renameLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				lua_pushboolean( L, m && m->renameLayer( ( int )luaL_checkinteger( L, 2 ), lua_checkstdstring( L, 3 ) ) );
				return 1;
			} },
		{ "getActiveLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				lua_pushinteger( L, m ? m->getActiveLayer() : 0 );
				return 1;
			} },
		{ "setActiveLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->setActiveLayer( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "layerIsVisible",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				lua_pushboolean( L, m && m->layerIsVisible( ( int )luaL_checkinteger( L, 2 ) ) );
				return 1;
			} },
		{ "setLayerVisibility",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->setLayerVisibility( ( int )luaL_checkinteger( L, 2 ), lua_toboolean( L, 3 ) != 0 );
				return 0;
			} },
		{ "addSelectionToLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->addSelectionToLayer( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "moveSelectionToLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->moveSelectionToLayer( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "removeSelectionFromLayer",
			[](lua_State* L)->int {
				auto* m	  = getLayerMgr();
				if( m )
					m->removeSelectionFromLayer( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "addNodeToLayer",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node ) {
					auto layered = std::dynamic_pointer_cast<scene::Layered>( ud->node );
					if( layered )
						layered->addToLayer( ( int )luaL_checkinteger( L, 3 ) );
				}
				return 0;
			} },
		{ "moveNodeToLayer",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node ) {
					auto layered = std::dynamic_pointer_cast<scene::Layered>( ud->node );
					if( layered )
						layered->moveToLayer( ( int )luaL_checkinteger( L, 3 ) );
				}
				return 0;
			} },
		{ "removeNodeFromLayer",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( ud && ud->node ) {
					auto layered = std::dynamic_pointer_cast<scene::Layered>( ud->node );
					if( layered )
						layered->removeFromLayer( ( int )luaL_checkinteger( L, 3 ) );
				}
				return 0;
			} },
		{ "getNodeLayers",
			[](lua_State* L)->int {
				lua_newtable( L );
				auto* ud	  = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
				if( !ud || !ud->node )
					return 1;
				auto layered = std::dynamic_pointer_cast<scene::Layered>( ud->node );
				if( !layered )
					return 1;
				int i	  = 1;
				for( int id : layered->getLayers() ) {
					lua_pushinteger( L, id );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ nullptr, nullptr }
	};
	lua_registerclass( L, "NeoRadiant.LayerSystem", mgr );

	lua_setglobal_object( L, "GlobalLayerSystem", this, "NeoRadiant.LayerSystem" );
}

} // namespace script
