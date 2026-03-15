#include "CameraInterface.h"
#include "../LuaHelper.h"
#include "MathInterface.h"
#include "icameraview.h"

namespace script
{

constexpr const char* META_CAMVIEW = "NeoRadiant.CameraView";

void CameraInterface::registerInterface( lua_State* L )
{
	static const luaL_Reg methods[] =
		{ { "getCameraOrigin",
				[](lua_State* L)->int {
				auto* c = lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW );
				lua_pushvec3( L, c->getCameraOrigin() );
				return 1;
		} },
		{ "setCameraOrigin",
			[](lua_State* L)->int {
				auto*	  c			= lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW );
				c->setCameraOrigin( *lua_checkvec3( L, 2 ) );
				return 0;
			} },
		{ "getCameraAngles",
			[](lua_State* L)->int {
				auto*	  c			= lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW );
				lua_pushvec3( L, c->getCameraAngles() );
				return 1;
			} },
		{ "setCameraAngles",
			[](lua_State* L)->int {
				auto*	  c			= lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW );
				c->setCameraAngles( *lua_checkvec3( L, 2 ) );
				return 0;
			} },
		{ "setOriginAndAngles",
			[](lua_State* L)->int {
				auto*	  c			= lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW );
				c->setOriginAndAngles( *lua_checkvec3( L, 2 ), *lua_checkvec3( L, 3 ) );
				return 0;
			} },
		{ "getRightVector",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->getRightVector() );
				return 1;
			} },
		{ "getUpVector",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->getUpVector() );
				return 1;
			} },
		{ "getForwardVector",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->getForwardVector() );
				return 1;
			} },
		{ "getFarClipPlaneDistance",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->getFarClipPlaneDistance() );
				return 1;
			} },
		{ "setFarClipPlaneDistance",
			[](lua_State* L)->int {
				lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->setFarClipPlaneDistance( ( float )luaL_checknumber( L, 2 ) );
				return 0;
			} },
		{ "refresh",
			[](lua_State* L)->int {
				lua_checkobject<camera::ICameraView>( L, 1, META_CAMVIEW )->queueDraw();
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_CAMVIEW, methods );

	static const luaL_Reg mgr[] =
		{ { "getActiveView",
			[](lua_State* L)->int {
			lua_pushobject( L, &GlobalCameraManager().getActiveView(), META_CAMVIEW );
			return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.CameraManager", mgr );

	lua_setglobal_object( L, "GlobalCameraManager", this, "NeoRadiant.CameraManager" );
}

}
