#include "RadiantInterface.h"

#include "../LuaHelper.h"

#include "iscenegraph.h"
#include "entitylib.h"

namespace script
{

void RadiantInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "findEntityByClassname",
			[](lua_State* L)->int {
				EntityNodeFindByClassnameWalker walker( lua_checkstdstring( L, 2 ) );
				GlobalSceneGraph().root()->traverse( walker );
				lua_pushscenenode( L, walker.getEntityNode() );
				return 1;
			} },
		{ "findEntityByName",
			[](lua_State* L)->int {
				std::string name		= lua_checkstdstring( L, 2 );
				scene::INodePtr found;
				GlobalSceneGraph().root()->foreachNode( [&]( const scene::INodePtr& n ) {
					auto* e = n->tryGetEntity();
					if( e && e->getKeyValue( "name" ) == name ) {
						found = n;
						return false;
					}
					return true;
				} );
				lua_pushscenenode( L, found );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.Radiant", methods );

	lua_setglobal_object( L, "Radiant", this, "NeoRadiant.Radiant" );
}

} // namespace script
