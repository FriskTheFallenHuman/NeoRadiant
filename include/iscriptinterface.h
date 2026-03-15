#pragma once

#include <memory>
#include <vector>
#include <utility>

// Forward-declare lua_State
struct lua_State;

namespace script
{

class IScriptInterface
{
public:
    using Ptr = std::shared_ptr<IScriptInterface>;

    virtual ~IScriptInterface() {}

    /**
     * Called by the scripting system to let this interface register its
     * types and globals into the given Lua state.
     */
    virtual void registerInterface(lua_State* L) = 0;
};

using NamedInterface = std::pair<std::string, IScriptInterface::Ptr>;
using NamedInterfaces = std::vector<NamedInterface>;

}
