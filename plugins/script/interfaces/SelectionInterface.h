#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "selectionlib.h"
#include <map>

#include "SceneGraphInterface.h"
#include "BrushInterface.h"

struct lua_State;

namespace script
{
class SelectionInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
