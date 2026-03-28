#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "iaasfile.h"

struct lua_State;

namespace script
{

class AasFileInterface : public IScriptInterface
{
public:
    void registerInterface(lua_State* L) override;
};

} // namespace script