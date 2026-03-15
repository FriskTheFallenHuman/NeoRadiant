#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ifilesystem.h"

#include "generic/callback.h"
#include "iarchive.h"
#include "itextstream.h"

struct lua_State;

namespace script
{

/**
 * greebo: This class registers the VFS interface with the
 * scripting system.
 */
class FileSystemInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
