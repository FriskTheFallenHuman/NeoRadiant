#pragma once

#include "iscriptinterface.h"
#include "inode.h"
#include "math/AABB.h"

struct lua_State;

namespace script
{

constexpr const char* META_SCENENODE = "NeoRadiant.SceneNode";

// Userdata type stored in Lua for scene::INode values.
// Defined here so all interface .cpp files can use it.
struct SceneNodeUD {
	scene::INodePtr node; // shared ownership — node lives as long as the userdata
};

// Push/check helpers used by all interfaces that deal with scene nodes.
void			lua_pushscenenode( lua_State* L, const scene::INodePtr& node );
scene::INodePtr lua_checkscenenode( lua_State* L, int idx );

// Register the SceneNode metatable (called once by SceneGraphInterface).
void			register_SceneNode( lua_State* L );

class SceneGraphInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};

} // namespace script
