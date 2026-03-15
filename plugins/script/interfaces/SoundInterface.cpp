#include "SoundInterface.h"
#include "../LuaHelper.h"
#include "isound.h"

namespace script
{

constexpr const char* META_SOUNDSHADER = "NeoRadiant.SoundShader";

void SoundInterface::registerInterface( lua_State* L )
{
	static luaL_Reg shaderMethods[] =
		{ { "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<ISoundShader>( L, 1, META_SOUNDSHADER )->getDeclName() );
				return 1;
			} },
		{ "getRadiusMin",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<ISoundShader>( L, 1, META_SOUNDSHADER )->getRadii().getMin() );
				return 1;
			} },
		{ "getRadiusMax",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<ISoundShader>( L, 1, META_SOUNDSHADER )->getRadii().getMax() );
				return 1;
			} },
		{ "getSoundFiles",
			[](lua_State* L)->int {
				const auto& files = lua_checkobject<ISoundShader>( L, 1, META_SOUNDSHADER )->getSoundFileList();
				lua_newtable( L );
				int i				= 1;
				for( const auto& f : files ) {
					lua_pushstdstring( L, f );
					lua_rawseti( L, -2, i++ );
				}
				return 1;
			} },
		{ "getDisplayFolder",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<ISoundShader>( L, 1, META_SOUNDSHADER )->getDisplayFolder() );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_SOUNDSHADER, shaderMethods );

	static luaL_Reg mgr[] =
		{ { "getSoundShader",
			[](lua_State* L)->int {
				auto s = GlobalSoundManager().getSoundShader( lua_checkstdstring( L, 2 ) );
				if( !s ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, s.get(), META_SOUNDSHADER );
				return 1;
			} },
		{ "playSound",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalSoundManager().playSound( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "stopSound",
			[](lua_State* L)->int {
				GlobalSoundManager().stopSound();
				return 0;
			} },
		{ "forEachShader",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalSoundManager().forEachShader( [&]( const ISoundShader::Ptr& s ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, s.get(), META_SOUNDSHADER );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.SoundManager", mgr );

	lua_setglobal_object( L, "GlobalSoundManager", this, "NeoRadiant.SoundManager" );
}

} // namespace script
