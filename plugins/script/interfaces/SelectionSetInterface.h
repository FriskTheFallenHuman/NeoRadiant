#pragma once

#include "iscriptinterface.h"
#include "iselectionset.h"

#include "SceneGraphInterface.h"

struct lua_State;

namespace script
{
class SelectionSetInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};
} // namespace script
