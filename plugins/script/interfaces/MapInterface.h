#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

#include "SceneGraphInterface.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class provides the script interface for the GlobalMap module.
 */
class MapInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
