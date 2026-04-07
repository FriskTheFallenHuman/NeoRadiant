#pragma once

#include "minilua.h"

#include <string>
#include <cstring>

namespace script
{

// -----------------------------------------------------------------------
// Generic userdata wrapper
// Stores a raw C++ pointer together with an ownership flag.
// When owned == true the __gc metamethod will call delete on the pointer
// (cast via the supplied deleter).  For objects owned by NeoRadiant's own
// C++ side (most things) owned == false and GC is a no-op.
// -----------------------------------------------------------------------
struct LuaObjectWrapper {
	void* ptr	= nullptr;
	bool  owned = false;
};

// -----------------------------------------------------------------------
// Push a non-owning pointer to a C++ object as a Lua userdata.
// The metatable named `metaName` must already exist in the registry.
// -----------------------------------------------------------------------
template<typename T>
inline void lua_pushobject( lua_State* L, T* obj, const char* metaName )
{
	if( !obj ) {
		lua_pushnil( L );
		return;
	}
	auto* w	 = static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) ) );
	w->ptr	 = static_cast<void*>( obj );
	w->owned = false;
	luaL_setmetatable( L, metaName );
}

// Push an owning pointer (Lua will delete it when GCed).
template<typename T>
inline void lua_pushowned( lua_State* L, T* obj, const char* metaName )
{
	auto* w	 = static_cast<LuaObjectWrapper*>( lua_newuserdata( L, sizeof( LuaObjectWrapper ) ) );
	w->ptr	 = static_cast<void*>( obj );
	w->owned = true;
	luaL_setmetatable( L, metaName );
}

// -----------------------------------------------------------------------
// Retrieve the typed pointer from a userdata at stack position idx.
// Raises a Lua error if the type doesn't match.
// -----------------------------------------------------------------------
template<typename T>
inline T* lua_checkobject( lua_State* L, int idx, const char* metaName )
{
	auto* w = static_cast<LuaObjectWrapper*>( luaL_checkudata( L, idx, metaName ) );
	return static_cast<T*>( w->ptr );
}

// Same but returns nullptr instead of raising an error (for optional args).
template<typename T>
inline T* lua_toobject( lua_State* L, int idx, const char* metaName )
{
	void* ud = lua_touserdata( L, idx );
	if( !ud ) return nullptr;
	if( !lua_getmetatable( L, idx ) ) return nullptr;
	luaL_getmetatable( L, metaName );
	bool ok = lua_rawequal( L, -1, -2 ) != 0;
	lua_pop( L, 2 );
	if( !ok ) return nullptr;
	return static_cast<T*>( static_cast<LuaObjectWrapper*>( ud )->ptr );
}

// -----------------------------------------------------------------------
// Register a class (metatable) in the Lua registry.
//
// Usage:
//   static const luaL_Reg methods[] = {
//       {"getX",  my_getX},
//       {nullptr, nullptr}
//   };
//   lua_registerclass(L, "NeoRadiant.Vector3", methods);
//
// After this call, any userdata whose metatable is "NeoRadiant.Vector3"
// can be called with obj:getX() from Lua.
//
// An optional __gc is also set that calls delete when owned == true.
// -----------------------------------------------------------------------
template<typename T>
inline int lua_default_gc( lua_State* L )
{
	auto* w = static_cast<LuaObjectWrapper*>( lua_touserdata( L, 1 ) );
	if( w && w->owned ) delete static_cast<T*>( w->ptr );
	return 0;
}

inline void lua_registerclass( lua_State* L, const char* metaName, const luaL_Reg* methods, lua_CFunction gcFunc = nullptr )
{
	luaL_newmetatable( L, metaName ); // push metatable

	// __index = metatable itself  →  obj:method() works
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_rawset( L, -3 );

	// Register methods into the metatable
	if( methods ) luaL_setfuncs( L, methods, 0 );

	// GC handler
	if( gcFunc ) {
		lua_pushstring( L, "__gc" );
		lua_pushcfunction( L, gcFunc );
		lua_rawset( L, -3 );
	}

	lua_pop( L, 1 ); // pop metatable
}

// -----------------------------------------------------------------------
// Convenience: set a global variable to a non-owning C++ object.
// e.g.  lua_setglobal_object(L, "GlobalSceneGraph", this, "NeoRadiant.SceneGraph")
// -----------------------------------------------------------------------
template<typename T>
inline void lua_setglobal_object( lua_State* L, const char* globalName, T* obj, const char* metaName )
{
	lua_pushobject( L, obj, metaName );
	lua_setglobal( L, globalName );
}

// -----------------------------------------------------------------------
// Stack helpers
// -----------------------------------------------------------------------
inline std::string lua_checkstdstring( lua_State* L, int idx )
{
	size_t		len = 0;
	const char* s	= luaL_checklstring( L, idx, &len );
	return std::string( s, len );
}

inline std::string lua_tostdstring( lua_State* L, int idx )
{
	size_t		len = 0;
	const char* s	= lua_tolstring( L, idx, &len );
	return s ? std::string( s, len ) : std::string();
}

inline void lua_pushstdstring( lua_State* L, const std::string& s )
{
	lua_pushlstring( L, s.data(), s.size() );
}

} // namespace script
