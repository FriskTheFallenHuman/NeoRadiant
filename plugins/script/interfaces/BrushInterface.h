#pragma once

#include "iscriptinterface.h"
#include "ibrush.h"

struct lua_State;

namespace script
{

constexpr const char* META_FACE	 = "NeoRadiant.Face";
constexpr const char* META_BRUSH = "NeoRadiant.BrushNode";

void				  lua_pushface( lua_State* L, IFace* face );
void				  lua_pushbrush( lua_State* L, const scene::INodePtr& brushNode );

class BrushInterface : public IScriptInterface
{
public:
	void	   registerInterface( lua_State* L ) override;

	// Factory method exposed as GlobalBrushCreator:createBrush()
	static int lua_createBrush( lua_State* L );
};

} // namespace script
