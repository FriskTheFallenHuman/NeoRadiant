#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ipatch.h"

#include "MathInterface.h"
#include "SceneGraphInterface.h"

struct lua_State;

namespace script
{
class PatchInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
