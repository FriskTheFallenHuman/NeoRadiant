#include "FxManagerInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_FXDECL	= "NeoRadiant.FxDeclaration";
constexpr const char* META_FXACTION = "NeoRadiant.FxAction";

void				  FxManagerInterface::registerInterface( lua_State* L )
{
	// IFxAction
	static luaL_Reg actionMethods[] =
		{ { "getActionType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getActionType() );
				return 1;
			} },
		{ "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getName() );
				return 1;
			} },
		{ "getDelay",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getDelayInSeconds() );
				return 1;
			} },
		{ "getRandomDelay",
			[](lua_State* L)->int {
				auto rd				= lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getRandomDelay();
				lua_pushnumber( L, rd.first );
				lua_pushnumber( L, rd.second );
				return 2;
			} },
		{ "getDuration",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getDurationInSeconds() );
				return 1;
			} },
		{ "getIgnoreMaster",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getIgnoreMaster() );
				return 1;
			} },
		{ "getShakeTime",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShakeTimeInSeconds() );
				return 1;
			} },
		{ "getShakeAmplitude",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShakeAmplitude() );
				return 1;
			} },
		{ "getShakeDistance",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShakeDistance() );
				return 1;
			} },
		{ "getShakeFalloff",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShakeFalloff() );
				return 1;
			} },
		{ "getShakeImpulse",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShakeImpulse() );
				return 1;
			} },
		{ "getNoShadows",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getNoShadows() );
				return 1;
			} },
		{ "getFireSiblingAction",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getFireSiblingAction() );
				return 1;
			} },
		{ "getRotate",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getRotate() );
				return 1;
			} },
		{ "getTrackOrigin",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getTrackOrigin() );
				return 1;
			} },
		{ "getRestart",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getRestart() );
				return 1;
			} },
		{ "getFadeIn",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getFadeInTimeInSeconds() );
				return 1;
			} },
		{ "getFadeOut",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getFadeOutTimeInSeconds() );
				return 1;
			} },
		{ "getDecalSize",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getDecalSize() );
				return 1;
			} },
		{ "getModelName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getModelName() );
				return 1;
			} },
		{ "getDecalMaterial",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getDecalMaterialName() );
				return 1;
			} },
		{ "getParticleTrackVelocity",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getParticleTrackVelocity() );
				return 1;
			} },
		{ "getSound",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getSoundShaderName() );
				return 1;
			} },
		{ "getShockwave",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxAction>( L, 1, META_FXACTION )->getShockwaveDefName() );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_FXACTION, actionMethods );

	// IFxDeclaration
	static luaL_Reg declMethods[] =
		{ { "getDeclName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxDeclaration>( L, 1, META_FXDECL )->getDeclName() );
				return 1;
			} },
		{ "getNumActions",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<fx::IFxDeclaration>( L, 1, META_FXDECL )->getNumActions() );
				return 1;
			} },
		{ "getAction",
			[](lua_State* L)->int {
				auto* d			  = lua_checkobject<fx::IFxDeclaration>( L, 1, META_FXDECL );
				std::size_t idx			  = ( std::size_t )luaL_checkinteger( L, 2 ) - 1;
				if( idx >= d->getNumActions() ) {
					lua_pushnil( L );
					return 1;
				}
				auto ptr			  = d->getAction( idx );
				if( !ptr ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, ptr.get(), META_FXACTION );
				return 1;
			} },
		{ "getBindTo",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<fx::IFxDeclaration>( L, 1, META_FXDECL )->getBindTo() );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_FXDECL, declMethods );

	// FxManager global
	static luaL_Reg mgr[] =
		{ { "findFx",
			[](lua_State* L)->int {
				auto fx = GlobalFxManager().findFx( lua_checkstdstring( L, 2 ) );
				if( !fx ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, fx.get(), META_FXDECL );
				return 1;
			} },
		{ "foreachFx",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalDeclarationManager().foreachDeclaration( decl::Type::Fx, [&]( const decl::IDeclaration::Ptr& d ) {
					auto fxd = std::dynamic_pointer_cast<fx::IFxDeclaration>( d );
					if( !fxd )
						return;
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, fxd.get(), META_FXDECL );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.FxManager", mgr );

	lua_setglobal_object( L, "GlobalFxManager", this, "NeoRadiant.FxManager" );
}

} // namespace script
