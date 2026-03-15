#include "EntityInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_ENTITY = "NeoRadiant.Entity";

static Entity* getNodeEntity( lua_State* L, int idx )
{
	auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, idx, "NeoRadiant.SceneNode" ) );
	if( !ud || !ud->node )
		return nullptr;
	return ud->node->tryGetEntity();
}

void EntityInterface::registerInterface( lua_State* L )
{
	luaL_getmetatable( L, "NeoRadiant.SceneNode" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		lua_pushboolean( L, ud && ud->node && Node_isEntity( ud->node ) );
		return 1;
	} );
	lua_setfield( L, -2, "isEntity" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		Entity* e = getNodeEntity( L, 1 );
		if( !e ) {
			lua_pushstring( L, "" );
			return 1;
		}
		lua_pushstdstring( L, e->getKeyValue( lua_checkstdstring( L, 2 ) ) );
		return 1;
	} );
	lua_setfield( L, -2, "getEntityKeyValue" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		Entity* e = getNodeEntity( L, 1 );
		if( e )
			e->setKeyValue( lua_checkstdstring( L, 2 ), lua_checkstdstring( L, 3 ) );
		return 0;
	} );
	lua_setfield( L, -2, "setEntityKeyValue" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		Entity* e = getNodeEntity( L, 1 );
		lua_pushboolean( L, e && e->isWorldspawn() );
		return 1;
	} );
	lua_setfield( L, -2, "isWorldSpawn" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		Entity* e = getNodeEntity( L, 1 );
		if( !e )
			return 0;
		luaL_checktype( L, 2, LUA_TFUNCTION );
		lua_pushvalue( L, 2 );
		int ref = luaL_ref( L, LUA_REGISTRYINDEX );
		e->forEachKeyValue( [&]( const std::string& key, const std::string& val ) {
			lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
			lua_pushstdstring( L, key );
			lua_pushstdstring( L, val );
			if( lua_pcall( L, 2, 0, 0 ) != LUA_OK )
				lua_pop( L, 1 );
		} );
		luaL_unref( L, LUA_REGISTRYINDEX, ref );
		return 0;
	} );
	lua_setfield( L, -2, "forEachEntityKeyValue" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		Entity* e = getNodeEntity( L, 1 );
		if( !e ) {
			lua_pushstring( L, "" );
			return 1;
		}
		auto ec = e->getEntityClass();
		lua_pushstdstring( L, ec ? ec->getDeclName() : std::string() );
		return 1;
	} );
	lua_setfield( L, -2, "getEntityClass" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* parent = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		auto* child	 = static_cast<SceneNodeUD*>( luaL_checkudata( L, 2, "NeoRadiant.SceneNode" ) );
		if( parent && parent->node && child && child->node )
			parent->node->addChildNode( child->node );
		return 0;
	} );
	lua_setfield( L, -2, "addChildNode" );

	lua_pop( L, 1 ); // pop SceneNode metatable

	// Entity creation
	static luaL_Reg creator[] =
		{ { "createEntity",
			[](lua_State* L)->int {
				auto eclass = GlobalEntityClassManager().findClass( lua_checkstdstring( L, 2 ) );
				if( !eclass ) {
					lua_pushnil( L );
					return 1;
				}
				auto node	  = GlobalEntityModule().createEntity( eclass );
				lua_pushscenenode( L, node );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.EntityCreator", creator );

	lua_setglobal_object( L, "GlobalEntityCreator", this, "NeoRadiant.EntityCreator" );
}

} // namespace script
