#pragma once
#include "iscriptinterface.h"
#include "ideclmanager.h"

struct lua_State;

namespace script
{
class DeclarationManagerInterface : public IScriptInterface
{
public:
	void registerInterface( lua_State* L ) override;
};
} // namespace script
