#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "igame.h"

struct lua_State;

namespace script
{
class GameInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
