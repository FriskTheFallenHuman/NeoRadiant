#include "PatchInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_PATCH = "NeoRadiant.PatchNode";

static void lua_pushpatchcontrol( lua_State* L, const PatchControl& pc )
{
	lua_newtable( L );
	lua_pushvec3( L, pc.vertex );
	lua_setfield( L, -2, "vertex" );
	lua_pushvec2( L, pc.texcoord );
	lua_setfield( L, -2, "texcoord" );
}

void PatchInterface::registerInterface( lua_State* L )
{
	static luaL_Reg patchMethods[] =
		{ { "getShader",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<IPatch>( L, 1, META_PATCH )->getShader() );
				return 1;
			} },
		{ "setShader",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->setShader( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "getWidth",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IPatch>( L, 1, META_PATCH )->getWidth() );
				return 1;
			} },
		{ "getHeight",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IPatch>( L, 1, META_PATCH )->getHeight() );
				return 1;
			} },
		{ "setDims",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->setDims( ( std::size_t )luaL_checkinteger( L, 2 ), ( std::size_t )luaL_checkinteger( L, 3 ) );
				return 0;
			} },
		{ "ctrlAt",
			[](lua_State* L)->int {
				auto* p			   = lua_checkobject<IPatch>( L, 1, META_PATCH );
				std::size_t col = ( std::size_t )luaL_checkinteger( L, 2 ) - 1;
				std::size_t row = ( std::size_t )luaL_checkinteger( L, 3 ) - 1;
				lua_pushpatchcontrol( L, p->ctrlAt( row, col ) );
				return 1;
			} },
		{ "controlPointsChanged",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->controlPointsChanged();
				return 0;
			} },
		{ "isValid",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<IPatch>( L, 1, META_PATCH )->isValid() );
				return 1;
			} },
		{ "subdivisionsFixed",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<IPatch>( L, 1, META_PATCH )->subdivisionsFixed() );
				return 1;
			} },
		{ "getSubdivisions",
			[](lua_State* L)->int {
				const auto& s			   = lua_checkobject<IPatch>( L, 1, META_PATCH )->getSubdivisions();
				lua_pushinteger( L, s.x() );
				lua_pushinteger( L, s.y() );
				return 2;
			} },
		{ "setFixedSubdivisions",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->setFixedSubdivisions( lua_toboolean( L, 2 ) != 0, Subdivisions( ( int )luaL_checkinteger( L, 3 ), ( int )luaL_checkinteger( L, 4 ) ) );
				return 0;
			} },
		{ "appendPoints",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->appendPoints( lua_toboolean( L, 2 ) != 0, lua_toboolean( L, 3 ) != 0 );
				return 0;
			} },
		{ "insertRemove",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->insertRemove( lua_toboolean( L, 2 ) != 0, lua_toboolean( L, 3 ) != 0, lua_toboolean( L, 4 ) != 0 );
				return 0;
			} },
		{ "invertMatrix",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->invertMatrix();
				return 0;
			} },
		{ "transposeMatrix",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->transposeMatrix();
				return 0;
			} },
		{ "normaliseTexture",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->normaliseTexture();
				return 0;
			} },
		{ "fitTexture",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->fitTexture( ( float )luaL_checknumber( L, 2 ), ( float )luaL_checknumber( L, 3 ) );
				return 0;
			} },
		{ "flipTexture",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->flipTexture( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "undoSave",
			[](lua_State* L)->int {
				lua_checkobject<IPatch>( L, 1, META_PATCH )->undoSave();
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_PATCH, patchMethods );

	luaL_getmetatable( L, "NeoRadiant.SceneNode" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		lua_pushboolean( L, ud && ud->node && Node_isPatch( ud->node ) );
		return 1;
	} );
	lua_setfield( L, -2, "isPatch" );

	lua_pushcfunction( L, [](lua_State* L)->int {
		auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, "NeoRadiant.SceneNode" ) );
		if( !ud || !ud->node || !Node_isPatch( ud->node ) ) {
			lua_pushnil( L );
			return 1;
		}
		IPatch* p = Node_getIPatch( ud->node );
		if( !p ) {
			lua_pushnil( L );
			return 1;
		}
		lua_pushobject( L, p, META_PATCH );
		return 1;
	} );
	lua_setfield( L, -2, "getPatch" );

	lua_pop( L, 1 );

	static luaL_Reg creator[] = {
		{ "createPatch",
			[](lua_State* L)->int {
				auto type = static_cast<patch::PatchDefType>( luaL_optinteger( L, 2, 0 ) );
				auto node	  = GlobalPatchModule().createPatch( type );
				lua_pushscenenode( L, node );
				return 1;
			} },
		{ nullptr, nullptr }
	};
	lua_registerclass( L, "NeoRadiant.PatchCreator", creator );

	lua_setglobal_object( L, "GlobalPatchCreator", this, "NeoRadiant.PatchCreator" );
}

} // namespace script
