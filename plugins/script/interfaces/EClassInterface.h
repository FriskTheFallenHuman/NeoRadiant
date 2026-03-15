#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ieclass.h"

#include "scene/EntityClass.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class provides the script interface for the GlobalEntityClassManager module.
 */
class EClassInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
