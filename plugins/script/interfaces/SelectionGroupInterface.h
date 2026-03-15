#pragma once

#include "iscriptinterface.h"

struct lua_State;

namespace script
{
class SelectionGroupInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};
} // namespace script
