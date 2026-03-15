#include "BrushInterface.h"
#include "SceneGraphInterface.h"

#include "../LuaHelper.h"
#include "../SceneNodeBuffer.h"

#include "MathInterface.h"

namespace script
{

// -----------------------------------------------------------------------
// Face
// -----------------------------------------------------------------------
void lua_pushface( lua_State* L, IFace* face )
{
	if( !face ) {
		lua_pushnil( L );
		return;
	}
	lua_pushobject( L, face, META_FACE );
}

static void register_Face( lua_State* L )
{
	static const luaL_Reg methods[] = {

		{ "getShader",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				lua_pushstdstring( L, face->getShader() );
				return 1;
			} },

		{ "setShader",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->setShader( lua_checkstdstring( L, 2 ) );
				return 0;
			} },

		{ "shiftTexdef",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->shiftTexdef( ( float )luaL_checknumber( L, 2 ), ( float )luaL_checknumber( L, 3 ) );
				return 0;
			} },

		{ "scaleTexdef",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->scaleTexdef( ( float )luaL_checknumber( L, 2 ), ( float )luaL_checknumber( L, 3 ) );
				return 0;
			} },

		{ "rotateTexdef",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->rotateTexdef( ( float )luaL_checknumber( L, 2 ) );
				return 0;
			} },

		{ "fitTexture",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->fitTexture( ( float )luaL_checknumber( L, 2 ), ( float )luaL_checknumber( L, 3 ) );
				return 0;
			} },

		{ "flipTexture",
			[](lua_State* L)->int {
				auto* face = lua_checkobject<IFace>( L, 1, META_FACE );
				face->flipTexture( ( unsigned int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },

		{ "normaliseTexture",
			[](lua_State* L)->int {
				lua_checkobject<IFace>( L, 1, META_FACE )->normaliseTexture();
				return 0;
			} },

		{ "undoSave",
			[](lua_State* L)->int {
				lua_checkobject<IFace>( L, 1, META_FACE )->undoSave();
				return 0;
			} },

		{ "getWinding",
			[](lua_State* L)->int {
				auto*			face = lua_checkobject<IFace>( L, 1, META_FACE );
				const IWinding& w	 = face->getWinding();

				lua_newtable( L ); // result table (1-indexed)
				for( std::size_t i = 0; i < w.size(); ++i ) {
					lua_newtable( L ); // individual vertex table

					lua_pushvec3( L, w[i].vertex );
					lua_setfield( L, -2, "vertex" );

					lua_pushvec2( L, w[i].texcoord );
					lua_setfield( L, -2, "texcoord" );

					lua_pushvec3( L, w[i].normal );
					lua_setfield( L, -2, "normal" );

					lua_pushvec3( L, w[i].tangent );
					lua_setfield( L, -2, "tangent" );

					lua_pushvec3( L, w[i].bitangent );
					lua_setfield( L, -2, "bitangent" );

					lua_pushinteger( L, ( lua_Integer )w[i].adjacent );
					lua_setfield( L, -2, "adjacent" );

					lua_rawseti( L, -2, ( lua_Integer )( i + 1 ) ); // 1-based
				}
				return 1;
			} },

		{ nullptr, nullptr }
	};

	lua_registerclass( L, META_FACE, methods );
}

// -----------------------------------------------------------------------
// BrushNode
// -----------------------------------------------------------------------

// Helpers that extract the IBrush* from the SceneNode userdata.
static IBrush* get_IBrush( lua_State* L, int idx )
{
	auto node = lua_checkscenenode( L, idx );
	if( !node )
		return nullptr;
	auto* brushNode = dynamic_cast<IBrushNode*>( node.get() );
	return brushNode ? &brushNode->getIBrush() : nullptr;
}

static void add_brush_methods_to_scenenode_meta( lua_State* L )
{
	// Extend the existing META_SCENENODE metatable with brush-specific methods.
	luaL_getmetatable( L, META_SCENENODE );
	if( lua_type( L, -1 ) != LUA_TTABLE ) {
		lua_pop( L, 1 );
		return; // SceneGraphInterface not yet registered; shouldn't happen.
	}

	auto set_method = [&]( const char* name, lua_CFunction fn ) {
		lua_pushstring( L, name );
		lua_pushcfunction( L, fn );
		lua_rawset( L, -3 );
	};

	set_method( "isBrush", [](lua_State* L)->int {
		auto node = lua_checkscenenode( L, 1 );
		lua_pushboolean( L, node ? Node_isBrush( node ) : false );
		return 1;
	} );

	set_method( "getBrush", [](lua_State* L)->int {
		// Returns self if it's a brush, else nil.
		auto node = lua_checkscenenode( L, 1 );
		if( node && Node_isBrush( node ) )
			lua_pushvalue( L, 1 );
		else
			lua_pushnil( L );
		return 1;
	} );

	set_method( "getNumFaces", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushinteger( L, b ? ( lua_Integer )b->getNumFaces() : 0 );
		return 1;
	} );

	set_method( "getFace", [](lua_State* L)->int {
		IBrush* b	= get_IBrush( L, 1 );
		auto	idx = ( std::size_t )luaL_checkinteger( L, 2 ) - 1; // Lua 1-based → C++ 0-based
		if( b && idx < b->getNumFaces() )
			lua_pushface( L, &b->getFace( idx ) );
		else
			lua_pushnil( L );
		return 1;
	} );

	set_method( "setShader", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		if( b )
			b->setShader( lua_checkstdstring( L, 2 ) );
		return 0;
	} );

	set_method( "hasShader", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushboolean( L, b ? b->hasShader( lua_checkstdstring( L, 2 ) ) : false );
		return 1;
	} );

	set_method( "hasVisibleMaterial", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushboolean( L, b ? b->hasVisibleMaterial() : false );
		return 1;
	} );

	set_method( "empty", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushboolean( L, b ? b->empty() : true );
		return 1;
	} );

	set_method( "hasContributingFaces", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushboolean( L, b ? b->hasContributingFaces() : false );
		return 1;
	} );

	set_method( "removeEmptyFaces", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		if( b )
			b->removeEmptyFaces();
		return 0;
	} );

	set_method( "undoSave", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		if( b )
			b->undoSave();
		return 0;
	} );

	set_method( "getDetailFlag", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		lua_pushinteger( L, b ? ( lua_Integer )b->getDetailFlag() : 0 );
		return 1;
	} );

	set_method( "setDetailFlag", [](lua_State* L)->int {
		IBrush* b = get_IBrush( L, 1 );
		if( b )
			b->setDetailFlag( static_cast<IBrush::DetailFlag>( luaL_checkinteger( L, 2 ) ) );
		return 0;
	} );

	lua_pop( L, 1 ); // pop metatable
}

// -----------------------------------------------------------------------
int BrushInterface::lua_createBrush( lua_State* L )
{
	scene::INodePtr node = GlobalBrushCreator().createBrush();
	SceneNodeBuffer::Instance().push_back( node );
	lua_pushscenenode( L, node );
	return 1;
}

// -----------------------------------------------------------------------
void BrushInterface::registerInterface( lua_State* L )
{
	register_Face( L );
	add_brush_methods_to_scenenode_meta( L );

	// GlobalBrushCreator table with createBrush()
	lua_newtable( L );
	lua_pushstring( L, "createBrush" );
	lua_pushcfunction( L, lua_createBrush );
	lua_rawset( L, -3 );
	lua_setglobal( L, "GlobalBrushCreator" );

	// Expose detail flag constants
	lua_pushinteger( L, ( lua_Integer )IBrush::Structural );
	lua_setglobal( L, "BRUSH_STRUCTURAL" );

	lua_pushinteger( L, ( lua_Integer )IBrush::Detail );
	lua_setglobal( L, "BRUSH_DETAIL" );
}

} // namespace script
