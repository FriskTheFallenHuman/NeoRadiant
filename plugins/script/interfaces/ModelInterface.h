#pragma once

#include "iscript.h"
#include "iscriptinterface.h"

#include "imodel.h"
#include "MathInterface.h"
#include "SceneGraphInterface.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class registers the model interface with the
 * scripting system.
 */
class ModelInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
