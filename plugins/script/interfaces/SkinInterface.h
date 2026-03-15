#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

#include "modelskin.h"

struct lua_State;

namespace script
{
class SkinInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
