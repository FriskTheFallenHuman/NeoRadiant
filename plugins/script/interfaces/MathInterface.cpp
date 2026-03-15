#include "MathInterface.h"
#include "../LuaHelper.h"

namespace script
{

// Helper: allocate a new Vector3 copy on the heap and push as userdata.
void lua_pushvec3( lua_State* L, const Vector3& v )
{
	auto*	 w = static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) + sizeof( Vector3 ) ) );
	// Store the Vector3 immediately after the wrapper.
	Vector3* stored = reinterpret_cast<Vector3*>( w + 1 );
	new( stored ) Vector3( v );
	w->ptr	 = stored;
	w->owned = false; // placement-new'd inside the userdata block; no heap delete needed
	luaL_setmetatable( L, META_VECTOR3 );
}

Vector3* lua_checkvec3( lua_State* L, int idx )
{
	auto* w = static_cast<LuaObjectWrapper*>( luaL_checkudata( L, idx, META_VECTOR3 ) );
	return static_cast<Vector3*>( w->ptr );
}

static void register_Vector3( lua_State* L )
{
	// Constructor: Vector3.new(x, y, z)
	lua_register( L, "Vector3", [](lua_State* L)->int {
		double x = luaL_optnumber( L, 1, 0.0 );
		double y = luaL_optnumber( L, 2, 0.0 );
		double z = luaL_optnumber( L, 3, 0.0 );
		lua_pushvec3( L, Vector3( x, y, z ) );
		return 1;
	} );

	static const luaL_Reg methods[] = {
		{ "x",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->x() );
				return 1;
			} },
		{ "y",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->y() );
				return 1;
			} },
		{ "z",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->z() );
				return 1;
			} },
		{ "setX",
			[](lua_State* L)->int {
				lua_checkvec3( L, 1 )->x() = luaL_checknumber( L, 2 );
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "setY",
			[](lua_State* L)->int {
				lua_checkvec3( L, 1 )->y() = luaL_checknumber( L, 2 );
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "setZ",
			[](lua_State* L)->int {
				lua_checkvec3( L, 1 )->z() = luaL_checknumber( L, 2 );
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "getLength",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->getLength() );
				return 1;
			} },
		{ "getLengthSquared",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->getLengthSquared() );
				return 1;
			} },
		{ "getNormalised",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkvec3( L, 1 )->getNormalised() );
				return 1;
			} },
		{ "normalise",
			[](lua_State* L)->int {
				lua_checkvec3( L, 1 )->normalise();
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "dot",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->dot( *lua_checkvec3( L, 2 ) ) );
				return 1;
			} },
		{ "cross",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkvec3( L, 1 )->cross( *lua_checkvec3( L, 2 ) ) );
				return 1;
			} },
		{ "crossProduct",
			[](lua_State* L)->int { // alias for compatibility
				lua_pushvec3( L, lua_checkvec3( L, 1 )->cross( *lua_checkvec3( L, 2 ) ) );
				return 1;
			} },
		{ "angle",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec3( L, 1 )->angle( *lua_checkvec3( L, 2 ) ) );
				return 1;
			} },
		{ "add",
			[](lua_State* L)->int {
				lua_pushvec3( L, *lua_checkvec3( L, 1 ) + *lua_checkvec3( L, 2 ) );
				return 1;
			} },
		{ "sub",
			[](lua_State* L)->int {
				lua_pushvec3( L, *lua_checkvec3( L, 1 ) - *lua_checkvec3( L, 2 ) );
				return 1;
			} },

		{ "__tostring",
			[](lua_State* L)->int {
				const Vector3& v			= *lua_checkvec3( L, 1 );
				lua_pushfstring( L, "(%f %f %f)", v.x(), v.y(), v.z() );
				return 1;
			} },

		{ nullptr, nullptr }
	};

	lua_registerclass( L, META_VECTOR3, methods );

	luaL_getmetatable( L, META_VECTOR3 );
	lua_pushstring( L, "__add" );
	lua_pushcfunction( L, [](lua_State* L)->int {
		lua_pushvec3( L, *lua_checkvec3( L, 1 ) + *lua_checkvec3( L, 2 ) );
		return 1;
	} );
	lua_rawset( L, -3 );

	lua_pushstring( L, "__sub" );
	lua_pushcfunction( L, [](lua_State* L)->int {
		lua_pushvec3( L, *lua_checkvec3( L, 1 ) - *lua_checkvec3( L, 2 ) );
		return 1;
	} );
	lua_rawset( L, -3 );

	lua_pushstring( L, "__unm" );
	lua_pushcfunction( L, [](lua_State* L)->int {
		const Vector3& v = *lua_checkvec3( L, 1 );
		lua_pushvec3( L, Vector3( -v.x(), -v.y(), -v.z() ) );
		return 1;
	} );
	lua_rawset( L, -3 );

	lua_pop( L, 1 ); // pop metatable
}

// Vector2
void lua_pushvec2( lua_State* L, const Vector2& v )
{
	auto*	 w		= static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) + sizeof( Vector2 ) ) );
	Vector2* stored = reinterpret_cast<Vector2*>( w + 1 );
	new( stored ) Vector2( v );
	w->ptr	 = stored;
	w->owned = false;
	luaL_setmetatable( L, META_VECTOR2 );
}

Vector2* lua_checkvec2( lua_State* L, int idx )
{
	auto* w = static_cast<LuaObjectWrapper*>( luaL_checkudata( L, idx, META_VECTOR2 ) );
	return static_cast<Vector2*>( w->ptr );
}

static void register_Vector2( lua_State* L )
{
	lua_register( L, "Vector2", [](lua_State* L)->int {
		double x = luaL_optnumber( L, 1, 0.0 );
		double y = luaL_optnumber( L, 2, 0.0 );
		lua_pushvec2( L, Vector2( x, y ) );
		return 1;
	} );

	static const luaL_Reg methods[] =
		{ { "x",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec2( L, 1 )->x() );
				return 1;
			} },
		{ "y",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec2( L, 1 )->y() );
				return 1;
			} },
		{ "setX",
			[](lua_State* L)->int {
				lua_checkvec2( L, 1 )->x() = luaL_checknumber( L, 2 );
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "setY",
			[](lua_State* L)->int {
				lua_checkvec2( L, 1 )->y() = luaL_checknumber( L, 2 );
				lua_pushvalue( L, 1 );
				return 1;
			} },
		{ "getLength",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec2( L, 1 )->getLength() );
				return 1;
			} },
		{ "getLengthSquared",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec2( L, 1 )->getLengthSquared() );
				return 1;
			} },
		{ "dot",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec2( L, 1 )->dot<double>( *lua_checkvec2( L, 2 ) ) );
				return 1;
			} },
		{ "__tostring",
			[](lua_State* L)->int {
				const Vector2& v			= *lua_checkvec2( L, 1 );
				lua_pushfstring( L, "(%f %f)", v.x(), v.y() );
				return 1;
			} },
		{ nullptr, nullptr } };

	lua_registerclass( L, META_VECTOR2, methods );
}

// Vector4
void lua_pushvec4( lua_State* L, const Vector4& v )
{
	auto*	 w		= static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) + sizeof( Vector4 ) ) );
	Vector4* stored = reinterpret_cast<Vector4*>( w + 1 );
	new( stored ) Vector4( v );
	w->ptr	 = stored;
	w->owned = false;
	luaL_setmetatable( L, META_VECTOR4 );
}

Vector4* lua_checkvec4( lua_State* L, int idx )
{
	auto* w = static_cast<LuaObjectWrapper*>( luaL_checkudata( L, idx, META_VECTOR4 ) );
	return static_cast<Vector4*>( w->ptr );
}

static void register_Vector4( lua_State* L )
{
	lua_register( L, "Vector4", [](lua_State* L)->int {
		double x = luaL_optnumber( L, 1, 0.0 );
		double y = luaL_optnumber( L, 2, 0.0 );
		double z = luaL_optnumber( L, 3, 0.0 );
		double w = luaL_optnumber( L, 4, 0.0 );
		lua_pushvec4( L, Vector4( x, y, z, w ) );
		return 1;
	} );

	// Quaternion is an alias
	lua_getglobal( L, "Vector4" );
	lua_setglobal( L, "Quaternion" );

	static const luaL_Reg methods[] =
		{ { "x",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec4( L, 1 )->x() );
				return 1;
			} },
		{ "y",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec4( L, 1 )->y() );
				return 1;
			} },
		{ "z",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec4( L, 1 )->z() );
				return 1;
			} },
		{ "w",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec4( L, 1 )->w() );
				return 1;
			} },
		{ "getVector3",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkvec4( L, 1 )->getVector3() );
				return 1;
			} },
		{ "getProjected",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkvec4( L, 1 )->getProjected() );
				return 1;
			} },
		{ "dot",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkvec4( L, 1 )->dot( *lua_checkvec4( L, 2 ) ) );
				return 1;
			} },
		{ "__tostring",
			[](lua_State* L)->int {
				const Vector4& v			= *lua_checkvec4( L, 1 );
				lua_pushfstring( L, "(%f %f %f %f)", v.x(), v.y(), v.z(), v.w() );
				return 1;
			} },
		{ nullptr, nullptr } };

	lua_registerclass( L, META_VECTOR4, methods );
}

// AABB
void lua_pushaabb( lua_State* L, const AABB& a )
{
	auto* w = static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) + sizeof( AABB ) ) );
	AABB* stored = reinterpret_cast<AABB*>( w + 1 );
	new( stored ) AABB( a );
	w->ptr	 = stored;
	w->owned = false;
	luaL_setmetatable( L, META_AABB );
}

AABB* lua_checkaabb( lua_State* L, int idx )
{
	auto* w = static_cast<LuaObjectWrapper*>( luaL_checkudata( L, idx, META_AABB ) );
	return static_cast<AABB*>( w->ptr );
}

static void register_AABB( lua_State* L )
{
	// AABB.new()  or  AABB.new(origin_vec3, extents_vec3)
	lua_register( L, "AABB", [](lua_State* L)->int {
		if( lua_gettop( L ) >= 2 ) {
			Vector3* origin	 = lua_checkvec3( L, 1 );
			Vector3* extents = lua_checkvec3( L, 2 );
			lua_pushaabb( L, AABB( *origin, *extents ) );
		} else {
			lua_pushaabb( L, AABB() );
		}
		return 1;
	} );

	static const luaL_Reg methods[] = {
		{ "getOrigin",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkaabb( L, 1 )->origin );
				return 1;
			} },
		{ "getExtents",
			[](lua_State* L)->int {
				lua_pushvec3( L, lua_checkaabb( L, 1 )->extents );
				return 1;
			} },
		{ "isValid",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkaabb( L, 1 )->isValid() );
				return 1;
			} },
		{ "getRadius",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkaabb( L, 1 )->getRadius() );
				return 1;
			} },
		{ "includePoint",
			[](lua_State* L)->int {
				lua_checkaabb( L, 1 )->includePoint( *lua_checkvec3( L, 2 ) );
				return 0;
			} },
		{ "includeAABB",
			[](lua_State* L)->int {
				lua_checkaabb( L, 1 )->includeAABB( *lua_checkaabb( L, 2 ) );
				return 0;
			} },
		{ "__tostring",
			[](lua_State* L)->int {
				const AABB& a = *lua_checkaabb( L, 1 );
				lua_pushfstring( L, "AABB(origin=(%f,%f,%f) extents=(%f,%f,%f))", a.origin.x(), a.origin.y(), a.origin.z(), a.extents.x(), a.extents.y(), a.extents.z() );
				return 1;
			} },
		{ nullptr, nullptr }
	};

	lua_registerclass( L, META_AABB, methods );
}

// MathInterface entry point
void MathInterface::registerInterface( lua_State* L )
{
	register_Vector3( L );
	register_Vector2( L );
	register_Vector4( L );
	register_AABB( L );

	// Vertex3 is an alias for Vector3 in scripts
	lua_getglobal( L, "Vector3" );
	lua_setglobal( L, "Vertex3" );
}

} // namespace script
