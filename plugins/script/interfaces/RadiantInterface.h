#pragma once

#include "iradiant.h"
#include "iscript.h"
#include "iscriptinterface.h"

#include "EntityInterface.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class registers the base set of interfaces, like
 * entity types, primitives, etc.
 */
class RadiantInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};

} // namespace script
