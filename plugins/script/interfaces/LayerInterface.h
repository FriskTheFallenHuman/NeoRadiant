#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ilayer.h"

#include "SceneGraphInterface.h"

struct lua_State;

namespace script
{
class LayerInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};

} // namespace script
