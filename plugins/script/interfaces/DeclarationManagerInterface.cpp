#include "DeclarationManagerInterface.h"
#include "../LuaHelper.h"
#include "ideclmanager.h"

namespace script
{

constexpr const char* META_DECL = "NeoRadiant.Declaration";

void DeclarationManagerInterface::registerInterface( lua_State* L )
{
	// IDeclaration wrapper
	static luaL_Reg declMethods[] =
		{ { "getDeclName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<decl::IDeclaration>( L, 1, META_DECL )->getDeclName() );
				return 1;
			} },
		{ "getDeclType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<decl::IDeclaration>( L, 1, META_DECL )->getDeclType() );
				return 1;
			} },
		{ "getDeclFilePath",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<decl::IDeclaration>( L, 1, META_DECL )->getDeclFilePath() );
				return 1;
			} },
		{ "getDeclSource",
			[](lua_State* L)->int {
				const auto& bs			  = lua_checkobject<decl::IDeclaration>( L, 1, META_DECL )->getDeclSource();
				lua_newtable( L );
				lua_pushstdstring( L, bs.name );
				lua_setfield( L, -2, "name" );
				lua_pushstdstring( L, bs.typeName );
				lua_setfield( L, -2, "typeName" );
				lua_pushstdstring( L, bs.contents );
				lua_setfield( L, -2, "contents" );
				lua_pushstdstring( L, bs.fileInfo.name );
				lua_setfield( L, -2, "filename" );
				return 1;
			} },
		{ "setDeclSource",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TTABLE );
				decl::DeclarationBlockSource bs;
				lua_getfield( L, 2, "name" );
				bs.name			  = lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "";
				lua_pop( L, 1 );
				lua_getfield( L, 2, "typeName" );
				bs.typeName		  = lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "";
				lua_pop( L, 1 );
				lua_getfield( L, 2, "contents" );
				bs.contents		  = lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "";
				lua_pop( L, 1 );
				lua_checkobject<decl::IDeclaration>( L, 1, META_DECL )->setDeclSource( bs );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_DECL, declMethods );

	// decl::Type enum constants
	lua_pushinteger( L, ( lua_Integer )decl::Type::Material );
	lua_setglobal( L, "DECL_MATERIAL" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::SoundShader );
	lua_setglobal( L, "DECL_SOUNDSHADER" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::EntityDef );
	lua_setglobal( L, "DECL_ENTITYDEF" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::ModelDef );
	lua_setglobal( L, "DECL_MODELDEF" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::Skin );
	lua_setglobal( L, "DECL_SKIN" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::Particle );
	lua_setglobal( L, "DECL_PARTICLE" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::Fx );
	lua_setglobal( L, "DECL_FX" );
	lua_pushinteger( L, ( lua_Integer )decl::Type::Table );
	lua_setglobal( L, "DECL_TABLE" );

	// DeclarationManager global
	static luaL_Reg mgr[] = {
		{ "findDeclaration",
			[](lua_State* L)->int {
				auto type = static_cast<decl::Type>( luaL_checkinteger( L, 2 ) );
				auto decl  = GlobalDeclarationManager().findDeclaration( type, lua_checkstdstring( L, 3 ) );
				if( !decl ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, decl.get(), META_DECL );
				return 1;
			} },
		{ "foreachDeclaration",
			[](lua_State* L)->int {
				auto type  = static_cast<decl::Type>( luaL_checkinteger( L, 2 ) );
				luaL_checktype( L, 3, LUA_TFUNCTION );
				lua_pushvalue( L, 3 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalDeclarationManager().foreachDeclaration( type, [&]( const decl::IDeclaration::Ptr& d ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, d.get(), META_DECL );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ "reloadDeclarations",
			[](lua_State* L)->int {
				GlobalDeclarationManager().reloadDeclarations();
				return 0;
			} },
		{ "saveDeclaration",
			[](lua_State* L)->int {
				auto* d	  = lua_checkobject<decl::IDeclaration>( L, 2, META_DECL );
				auto decl  = GlobalDeclarationManager().findDeclaration( d->getDeclType(), d->getDeclName() );
				if( decl )
					GlobalDeclarationManager().saveDeclaration( decl );
				return 0;
			} },
		{ "renameDeclaration",
			[](lua_State* L)->int {
				auto type  = static_cast<decl::Type>( luaL_checkinteger( L, 2 ) );
				lua_pushboolean( L, GlobalDeclarationManager().renameDeclaration( type, lua_checkstdstring( L, 3 ), lua_checkstdstring( L, 4 ) ) );
				return 1;
			} },
		{ "removeDeclaration",
			[](lua_State* L)->int {
				auto type  = static_cast<decl::Type>( luaL_checkinteger( L, 2 ) );
				GlobalDeclarationManager().removeDeclaration( type, lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ nullptr, nullptr }
	};
	lua_registerclass( L, "NeoRadiant.DeclarationManager", mgr );
	lua_setglobal_object( L, "GlobalDeclarationManager", this, "NeoRadiant.DeclarationManager" );
}

} // namespace script
