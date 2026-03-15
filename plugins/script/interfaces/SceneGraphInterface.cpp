#include "SceneGraphInterface.h"

#include "../LuaHelper.h"
#include "MathInterface.h"

#include "iscenegraph.h"
#include "iselection.h"
#include "scenelib.h"
#include "debugging/ScenegraphUtils.h"

#include <memory>

namespace script
{
void lua_pushscenenode( lua_State* L, const scene::INodePtr& node )
{
	if( !node ) {
		lua_pushnil( L );
		return;
	}
	auto* ud = static_cast<SceneNodeUD*>( lua_newuserdata( L, sizeof( SceneNodeUD ) ) );
	new( ud ) SceneNodeUD { node };
	luaL_setmetatable( L, META_SCENENODE );
}

scene::INodePtr lua_checkscenenode( lua_State* L, int idx )
{
	auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, idx, META_SCENENODE ) );
	return ud->node;
}

// -----------------------------------------------------------------------
void register_SceneNode( lua_State* L )
{
	static const luaL_Reg methods[] = {

		{ "isNull",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( luaL_checkudata( L, 1, META_SCENENODE ) );
				lua_pushboolean( L, ud->node == nullptr );
				return 1;
			} },

		{ "getNodeType",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				lua_pushstdstring( L, node ? getNameForNodeType( node->getNodeType() ) : "null" );
				return 1;
			} },

		{ "getWorldAABB",
			[](lua_State* L)->int {
				auto		node = lua_checkscenenode( L, 1 );
				static AABB empty;
				lua_pushaabb( L, node ? node->worldAABB() : empty );
				return 1;
			} },

		{ "getParent",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				lua_pushscenenode( L, node ? node->getParent() : scene::INodePtr() );
				return 1;
			} },

		{ "removeFromParent",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				if( node )
					scene::removeNodeFromParent( node );
				return 0;
			} },

		{ "addToContainer",
			[](lua_State* L)->int {
				auto node	   = lua_checkscenenode( L, 1 );
				auto container = lua_checkscenenode( L, 2 );
				if( node && container )
					scene::addNodeToContainer( node, container );
				return 0;
			} },

		{ "isSelected",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				if( !node ) {
					lua_pushboolean( L, 0 );
					return 1;
				}
				auto sel = scene::node_cast<ISelectable>( node );
				lua_pushboolean( L, sel ? sel->isSelected() : false );
				return 1;
			} },

		{ "setSelected",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				bool val  = lua_toboolean( L, 2 ) != 0;
				if( node ) {
					auto sel = scene::node_cast<ISelectable>( node );
					if( sel )
						sel->setSelected( val );
				}
				return 0;
			} },

		{ "invertSelected",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				if( node ) {
					auto sel = scene::node_cast<ISelectable>( node );
					if( sel )
						sel->setSelected( !sel->isSelected() );
				}
				return 0;
			} },
		{ "traverse",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				luaL_checktype( L, 2, LUA_TFUNCTION );

				if( !node )
					return 0;

				// Build a C++ visitor that calls the Lua function.
				struct LuaVisitor : public scene::NodeVisitor {
					lua_State* L;
					int		   funcRef;

					LuaVisitor( lua_State* l, int ref ) :
						L( l ),
						funcRef( ref )
					{
					}

					bool pre( const scene::INodePtr& child ) override
					{
						lua_rawgeti( L, LUA_REGISTRYINDEX, funcRef );
						lua_pushscenenode( L, child );
						if( lua_pcall( L, 1, 1, 0 ) != LUA_OK ) {
							lua_pop( L, 1 );
							return false;
						}
						bool cont = lua_toboolean( L, -1 ) != 0;
						lua_pop( L, 1 );
						return cont;
					}
				};

				lua_pushvalue( L, 2 );
				int		   ref = luaL_ref( L, LUA_REGISTRYINDEX );
				LuaVisitor visitor( L, ref );
				node->traverse( visitor );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "__tostring",
			[](lua_State* L)->int {
				auto node = lua_checkscenenode( L, 1 );
				if( !node ) {
					lua_pushstring( L, "SceneNode(null)" );
					return 1;
				}
				lua_pushfstring( L, "SceneNode(%s)", getNameForNodeType( node->getNodeType() ).c_str() );
				return 1;
			} },
		{ "__gc",
			[](lua_State* L)->int {
				auto* ud = static_cast<SceneNodeUD*>( lua_touserdata( L, 1 ) );
				if( ud )
					ud->~SceneNodeUD();
				return 0;
			} },

		{ nullptr, nullptr }
	};

	lua_registerclass( L, META_SCENENODE, methods );
}

// SceneGraphInterface
constexpr const char* META_SCENEGRAPH = "NeoRadiant.SceneGraph";

void SceneGraphInterface::registerInterface( lua_State* L )
{
	register_SceneNode( L );

	static const luaL_Reg sgMethods[] =
		{ { "root",
			[](lua_State* L)->int {
				lua_pushscenenode( L, GlobalSceneGraph().root() );
				return 1;
			} },
		{ nullptr, nullptr } };

	lua_registerclass( L, META_SCENEGRAPH, sgMethods );

	lua_setglobal_object( L, "GlobalSceneGraph", this, META_SCENEGRAPH );
}

} // namespace script
