#pragma once
#include "iscript.h"
#include "iscriptinterface.h"
#include "ui/idialogmanager.h"

struct lua_State;

namespace script
{
/**
 * greebo: This class provides the script interface for the DialogManager class (UIManager module).
 */
class DialogInterface : public IScriptInterface
{
public:
	// IScriptInterface implementation
	void registerInterface( lua_State* L ) override;
};

} // namespace script
