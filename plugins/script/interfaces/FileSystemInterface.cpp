#include "FileSystemInterface.h"
#include "../LuaHelper.h"

namespace script
{

void FileSystemInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] = {
		{ "forEachFile",
			[](lua_State* L)->int {
				std::string base = lua_checkstdstring( L, 2 );
				std::string ext = lua_checkstdstring( L, 3 );
				luaL_checktype( L, 4, LUA_TFUNCTION );
				std::size_t depth = ( std::size_t )luaL_optinteger( L, 5, 99 );

				lua_pushvalue( L, 4 );
				int ref = luaL_ref( L, LUA_REGISTRYINDEX );

				GlobalFileSystem().forEachFile(
					base,
					ext,
					[L, ref]( const vfs::FileInfo& fi ) {
						lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
						lua_pushstdstring( L, fi.name );
						if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
							lua_pop( L, 1 );
					},
					depth );

				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },

		{ "readTextFile",
			[](lua_State* L)->int {
				auto f = GlobalFileSystem().openTextFile( lua_checkstdstring( L, 2 ) );
				if( !f ) {
					lua_pushstring( L, "" );
					return 1;
				}
				TextInputStream& s = f->getInputStream();
				std::string text;
				char	  buf[4096];
				std::size_t n;
				while( ( n = s.read( buf, sizeof( buf ) ) ) > 0 )
					text.append( buf, n );
				lua_pushstdstring( L, text );
				return 1;
			} },

		{ "getFileCount",
			[](lua_State* L)->int {
				lua_pushinteger( L, GlobalFileSystem().getFileCount( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },

		{ "findFile",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalFileSystem().findFile( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },

		{ "findRoot",
			[](lua_State* L)->int {
				lua_pushstdstring( L, GlobalFileSystem().findRoot( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },

		{ nullptr, nullptr }
	};
	lua_registerclass( L, "NeoRadiant.FileSystem", methods );

	lua_setglobal_object( L, "GlobalFileSystem", this, "NeoRadiant.FileSystem" );
}

} // namespace script
