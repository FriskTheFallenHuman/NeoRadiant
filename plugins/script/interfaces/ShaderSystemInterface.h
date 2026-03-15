#pragma once

#include "ishaders.h"
#include "iscript.h"
#include "iscriptinterface.h"

#include "MathInterface.h"

struct lua_State;

namespace script
{
class ShaderSystemInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
