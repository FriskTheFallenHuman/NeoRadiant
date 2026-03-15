#pragma once

#include "iscriptinterface.h"
#include "ifx.h"

struct lua_State;

namespace script
{
/**
 * Exposes the GlobalFxManager interface to scripts
 */
class FxManagerInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

}
