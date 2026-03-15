#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

struct lua_State;

namespace script
{
class CommandSystemInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};

} // namespace script
