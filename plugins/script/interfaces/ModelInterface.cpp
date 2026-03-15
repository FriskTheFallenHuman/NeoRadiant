#include "ModelInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_MODEL	 = "NeoRadiant.Model";
constexpr const char* META_MODELSURF = "NeoRadiant.ModelSurface";

void				  ModelInterface::registerInterface( lua_State* L )
{
	// IModelSurface
	static luaL_Reg surfMethods[] =
		{ { "getNumVertices",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<model::IModelSurface>( L, 1, META_MODELSURF )->getNumVertices() );
				return 1;
			} },
		{ "getNumTriangles",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<model::IModelSurface>( L, 1, META_MODELSURF )->getNumTriangles() );
				return 1;
			} },
		{ "getDefaultMaterial",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<model::IModelSurface>( L, 1, META_MODELSURF )->getDefaultMaterial() );
				return 1;
			} },
		{ "getActiveMaterial",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<model::IModelSurface>( L, 1, META_MODELSURF )->getActiveMaterial() );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_MODELSURF, surfMethods );

	// IModel
	static luaL_Reg modelMethods[] =
		{ { "getFilename",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<model::IModel>( L, 1, META_MODEL )->getFilename() );
				return 1;
			} },
		{ "getModelPath",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<model::IModel>( L, 1, META_MODEL )->getModelPath() );
				return 1;
			} },
		{ "getSurfaceCount",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<model::IModel>( L, 1, META_MODEL )->getSurfaceCount() );
				return 1;
			} },
		{ "getVertexCount",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<model::IModel>( L, 1, META_MODEL )->getVertexCount() );
				return 1;
			} },
		{ "getPolyCount",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<model::IModel>( L, 1, META_MODEL )->getPolyCount() );
				return 1;
			} },
		{ "getSurface",
			[](lua_State* L)->int {
				auto* m			   = lua_checkobject<model::IModel>( L, 1, META_MODEL );
				int idx			   = ( int )luaL_checkinteger( L, 2 ) - 1;
				if( idx < 0 || idx >= m->getSurfaceCount() ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, &const_cast<model::IModelSurface&>( m->getSurface( ( unsigned )idx ) ), META_MODELSURF );
				return 1;
			} },
		{ "getLocalBounds",
			[](lua_State* L)->int {
				const AABB& bb			   = lua_checkobject<model::IModel>( L, 1, META_MODEL )->localAABB();
				lua_pushaabb( L, bb );
				return 1;
			} },
		{ "getMaterials",
			[](lua_State* L)->int {
				auto* m			   = lua_checkobject<model::IModel>( L, 1, META_MODEL );
				lua_newtable( L );
				for( int i = 0; i < m->getSurfaceCount(); ++i ) {
					lua_pushstdstring( L, m->getSurface( ( unsigned )i ).getActiveMaterial() );
					lua_rawseti( L, -2, i + 1 );
				}
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_MODEL, modelMethods );

	luaL_getmetatable( L, "NeoRadiant.SceneNode" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		lua_pushboolean( L, ud && ud->node && Node_getModel( ud->node ) != nullptr );
		return 1;
	} );
	lua_setfield( L, -2, "isModel" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		if( !ud || !ud->node ) {
			lua_pushnil( L );
			return 1;
		}
		auto modelNode = Node_getModel( ud->node );
		if( !modelNode ) {
			lua_pushnil( L );
			return 1;
		}
		lua_pushobject( L, &modelNode->getIModel(), META_MODEL );
		return 1;
	} );
	lua_setfield( L, -2, "getModel" );

	lua_pop( L, 1 ); // pop metatable

	// ModelCache
	static luaL_Reg cache[] =
		{ { "getModel",
			[](lua_State* L)->int {
				auto m = GlobalModelCache().getModel( lua_checkstdstring( L, 2 ) );
				if( !m ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, m.get(), META_MODEL );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.ModelCache", cache );

	lua_setglobal_object( L, "GlobalModelCache", this, "NeoRadiant.ModelCache" );
}

} // namespace script
