#include "SkinInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_SKIN = "NeoRadiant.ModelSkin";

void SkinInterface::registerInterface( lua_State* L )
{
	static luaL_Reg skinMethods[] =
		{ { "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<decl::ISkin>( L, 1, META_SKIN )->getDeclName() );
				return 1;
			} },
		{ "getRemap",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<decl::ISkin>( L, 1, META_SKIN )->getRemap( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "getAllRemappings",
			[](lua_State* L)->int {
				auto* s			  = lua_checkobject<decl::ISkin>( L, 1, META_SKIN );
				const auto& remaps		  = s->getAllRemappings();
				lua_newtable( L );
				int i			  = 1;
				for( const auto& r : remaps ) {
					lua_newtable( L );
					lua_pushstdstring( L, r.Original );
					lua_setfield( L, -2, "original" );
					lua_pushstdstring( L, r.Replacement );
					lua_setfield( L, -2, "replacement" );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_SKIN, skinMethods );

	static luaL_Reg lib[] =
		{ { "findSkin",
			[](lua_State* L)->int {
				auto s = GlobalModelSkinCache().findSkin( lua_checkstdstring( L, 2 ) );
				if( !s ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, s.get(), META_SKIN );
				return 1;
			} },
		{ "getSkinsForModel",
			[](lua_State* L)->int {
				const auto& skins = GlobalModelSkinCache().getSkinsForModel( lua_checkstdstring( L, 2 ) );
				lua_newtable( L );
				int i	  = 1;
				for( const auto& name : skins ) {
					lua_pushstdstring( L, name );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ "getAllSkins",
			[](lua_State* L)->int {
				const auto& skins = GlobalModelSkinCache().getAllSkins();
				lua_newtable( L );
				int i	  = 1;
				for( const auto& name : skins ) {
					lua_pushstdstring( L, name );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ nullptr, nullptr }
	};
	lua_registerclass( L, "NeoRadiant.ModelSkinCache", lib );

	lua_setglobal_object( L, "GlobalModelSkinCache", this, "NeoRadiant.ModelSkinCache" );
}

} // namespace script
