#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ientity.h"
#include "ieclass.h"
#include "inode.h"

#include "scene/Entity.h"

#include "EClassInterface.h"
#include "SceneGraphInterface.h"

#include "entitylib.h"

struct lua_State;

namespace script
{
/**
 * greebo: This class registers the entity interface with the
 * scripting system.
 */
class EntityInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
