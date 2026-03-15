#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class provides the script interface for the GlobalGrid module.
 */
class GridInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
