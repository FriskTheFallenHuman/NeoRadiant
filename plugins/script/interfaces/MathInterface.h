#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/AABB.h"

struct lua_State;

namespace script
{
constexpr const char* META_VECTOR2 = "NeoRadiant.Vector2";
constexpr const char* META_VECTOR3 = "NeoRadiant.Vector3";
constexpr const char* META_VECTOR4 = "NeoRadiant.Vector4";
constexpr const char* META_AABB	   = "NeoRadiant.AABB";

// Push/check helpers declared here so other interfaces can push math objects.
void				  lua_pushvec2( lua_State* L, const Vector2& v );
void				  lua_pushvec3( lua_State* L, const Vector3& v );
void				  lua_pushvec4( lua_State* L, const Vector4& v );
void				  lua_pushaabb( lua_State* L, const AABB& a );

Vector2*			  lua_checkvec2( lua_State* L, int idx );
Vector3*			  lua_checkvec3( lua_State* L, int idx );
Vector4*			  lua_checkvec4( lua_State* L, int idx );
AABB*				  lua_checkaabb( lua_State* L, int idx );

class MathInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
