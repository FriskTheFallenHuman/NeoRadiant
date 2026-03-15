#pragma once

#include "iscript.h"
#include "iscriptinterface.h"
#include "ScriptCommand.h"

#include "minilua.h"
#include "LuaHelper.h"

#include <string>
#include <memory>
#include <vector>

namespace script
{

/**
 * Manages the Lua interpreter and exposes NeoRadiant's C++ interfaces to
 * scripts.
 */
class LuaModule final
{
public:
	LuaModule();
	~LuaModule();

	// Starts the interpreter.  Call after all interfaces have been added.
	void			   initialise();

	// Execute a string of Lua code.
	ExecutionResultPtr executeString( const std::string& code );

	// Execute a script file (path relative to the scripts base path).
	void			   executeScriptFile( const std::string& scriptBasePath, const std::string& relativeScriptPath, bool setExecuteCommandAttr );

	// Add a named interface (must be called before initialise()).
	void			   addInterface( const NamedInterface& iface );

	// Try to parse a script file as a command definition.
	// Returns an empty ptr if the file doesn't export __commandName__.
	ScriptCommand::Ptr createScriptCommand( const std::string& scriptBasePath, const std::string& relativeScriptPath );

	// Direct access to the interpreter state (for advanced use).
	lua_State*		   getState() { return _L; }

private:
	lua_State*		_L = nullptr;

	NamedInterfaces _namedInterfaces;
	bool			_initialised = false;

	// Accumulated output/error from the last execution.
	std::string		_outputBuffer;
	std::string		_errorBuffer;

	void			registerStandardInterfaces();

	// Lua print() override that captures to _outputBuffer.
	static int		lua_print_override( lua_State* L );

	// Error message handler pushed onto the stack before pcall.
	static int		lua_msgh( lua_State* L );
};

} // namespace script
