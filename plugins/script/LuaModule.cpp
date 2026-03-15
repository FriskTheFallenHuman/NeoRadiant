#include "LuaModule.h"

#include "itextstream.h"
#include "os/file.h"

#include <stdexcept>
#include <sstream>

namespace script
{

// Constructor / Destructor
LuaModule::LuaModule()
{
}

LuaModule::~LuaModule()
{
	if( _L ) {
		lua_close( _L );
		_L = nullptr;
	}
}

// Initialise
void LuaModule::initialise()
{
	_L = luaL_newstate();
	if( !_L ) {
		rError() << "Failed to create Lua state" << std::endl;
		return;
	}

	// Open standard libraries (math, string, table, io, os, …)
	luaL_openlibs( _L );

	// Override print() so output goes to our buffer and the NR log.
	lua_pushcfunction( _L, lua_print_override );
	lua_setglobal( _L, "print" );

	// Stash a pointer to this LuaModule in the Lua registry so the print
	// override and other static callbacks can reach it.
	lua_pushlightuserdata( _L, static_cast<void*>( this ) );
	lua_setfield( _L, LUA_REGISTRYINDEX, "__LuaModule__" );

	// Register all C++ interfaces.
	for( const auto& namedIface : _namedInterfaces ) {
		try {
			namedIface.second->registerInterface( _L );
		} catch( const std::exception& ex ) {
			rError() << "Lua: error registering interface '" << namedIface.first << "': " << ex.what() << std::endl;
		}
	}

	_initialised = true;
	rMessage() << "Lua 5.5 interpreter ready." << std::endl;
}

// Execute string
ExecutionResultPtr LuaModule::executeString( const std::string& code )
{
	auto result			  = std::make_shared<ExecutionResult>();
	result->errorOccurred = false;

	_outputBuffer.clear();
	_errorBuffer.clear();

	// Push error message handler, then load and call the chunk.
	int base = lua_gettop( _L );
	lua_pushcfunction( _L, lua_msgh );

	if( luaL_loadstring( _L, code.c_str() ) != LUA_OK ) {
		// Compile error
		result->errorOccurred = true;
		_errorBuffer		  = lua_tostdstring( _L, -1 );
		lua_pop( _L, 2 ); // error string + msgh
	} else {
		// Execute
		if( lua_pcall( _L, 0, 0, base + 1 ) != LUA_OK ) {
			result->errorOccurred = true;
			_errorBuffer		  = lua_tostdstring( _L, -1 );
			lua_pop( _L, 1 ); // error string
		}
		lua_pop( _L, 1 ); // msgh
	}

	if( !_errorBuffer.empty() )
		rError() << "Lua error: " << _errorBuffer << std::endl;

	result->output = _outputBuffer;
	if( !_errorBuffer.empty() )
		result->output += "\n" + _errorBuffer;

	_outputBuffer.clear();
	_errorBuffer.clear();

	return result;
}

// Execute script file
void LuaModule::executeScriptFile( const std::string& scriptBasePath, const std::string& relativeScriptPath, bool setExecuteCommandAttr )
{
	std::string fullPath = scriptBasePath + relativeScriptPath;

	if( !os::fileOrDirExists( fullPath ) ) {
		rError() << "Lua: script file not found: " << fullPath << std::endl;
		return;
	}

	// If requested, set the global __executeCommand__ = true before running.
	if( setExecuteCommandAttr ) {
		lua_pushboolean( _L, 1 );
		lua_setglobal( _L, "__executeCommand__" );
	} else {
		lua_pushboolean( _L, 0 );
		lua_setglobal( _L, "__executeCommand__" );
	}

	int base = lua_gettop( _L );
	lua_pushcfunction( _L, lua_msgh );

	if( luaL_loadfile( _L, fullPath.c_str() ) != LUA_OK ) {
		rError() << "Lua: compile error in " << relativeScriptPath << ": " << lua_tostring( _L, -1 ) << std::endl;
		lua_pop( _L, 2 );
		return;
	}

	if( lua_pcall( _L, 0, 0, base + 1 ) != LUA_OK ) {
		rError() << "Lua: runtime error in " << relativeScriptPath << ": " << lua_tostring( _L, -1 ) << std::endl;
		lua_pop( _L, 1 );
	}

	lua_pop( _L, 1 ); // msgh

	// Reset the flag after execution.
	lua_pushboolean( _L, 0 );
	lua_setglobal( _L, "__executeCommand__" );
}

// Interfaces
void LuaModule::addInterface( const NamedInterface& iface )
{
	for( const auto& ni : _namedInterfaces ) {
		if( ni.first == iface.first ) {
			rError() << "Lua: interface '" << iface.first << "' already registered" << std::endl;
			return;
		}
	}

	_namedInterfaces.emplace_back( iface );

	// If already initialised, register immediately.
	if( _initialised ) {
		try {
			iface.second->registerInterface( _L );
		} catch( const std::exception& ex ) {
			rError() << "Lua: error registering interface '" << iface.first << "': " << ex.what() << std::endl;
		}
	}
}

// Script command discovery
ScriptCommand::Ptr LuaModule::createScriptCommand( const std::string& scriptBasePath, const std::string& relativeScriptPath )
{
	std::string fullPath = scriptBasePath + relativeScriptPath;

	if( !os::fileOrDirExists( fullPath ) )
		return {};

	// Load and execute with __executeCommand__ = false.
	lua_pushboolean( _L, 0 );
	lua_setglobal( _L, "__executeCommand__" );

	int base = lua_gettop( _L );
	lua_pushcfunction( _L, lua_msgh );

	if( luaL_loadfile( _L, fullPath.c_str() ) != LUA_OK ) {
		rError() << "Lua: compile error in " << relativeScriptPath << ": " << lua_tostring( _L, -1 ) << std::endl;
		lua_pop( _L, 2 );
		return {};
	}

	if( lua_pcall( _L, 0, 0, base + 1 ) != LUA_OK ) {
		rError() << "Lua: runtime error in " << relativeScriptPath << ": " << lua_tostring( _L, -1 ) << std::endl;
		lua_pop( _L, 1 );
		lua_pop( _L, 1 ); // msgh
		return {};
	}
	lua_pop( _L, 1 ); // msgh

	// Read __commandName__ and __commandDisplayName__ from globals.
	std::string cmdName, cmdDisplayName;

	lua_getglobal( _L, "__commandName__" );
	if( lua_type( _L, -1 ) == LUA_TSTRING )
		cmdName = lua_tostring( _L, -1 );
	lua_pop( _L, 1 );

	lua_getglobal( _L, "__commandDisplayName__" );
	if( lua_type( _L, -1 ) == LUA_TSTRING )
		cmdDisplayName = lua_tostring( _L, -1 );
	lua_pop( _L, 1 );

	if( cmdName.empty() ) {
		rError() << "Lua: " << relativeScriptPath << " does not export __commandName__" << std::endl;
		return {};
	}

	if( cmdDisplayName.empty() )
		cmdDisplayName = cmdName;

	return std::make_shared<ScriptCommand>( cmdName, cmdDisplayName, relativeScriptPath );
}

// Static callbacks
int LuaModule::lua_print_override( lua_State* L )
{
	// Retrieve the LuaModule pointer from the registry.
	lua_getfield( L, LUA_REGISTRYINDEX, "__LuaModule__" );
	auto* self = static_cast<LuaModule*>( lua_touserdata( L, -1 ) );
	lua_pop( L, 1 );

	int			n = lua_gettop( L );
	std::string line;
	for( int i = 1; i <= n; ++i ) {
		if( i > 1 )
			line += "\t";
		size_t		len = 0;
		const char* s	= luaL_tolstring( L, i, &len ); // calls tostring()
		line.append( s, len );
		lua_pop( L, 1 ); // luaL_tolstring pushes a string
	}
	line += "\n";

	if( self )
		self->_outputBuffer += line;

	rMessage() << line;
	return 0;
}

int LuaModule::lua_msgh( lua_State* L )
{
	// Append a traceback to the error message.
	const char* msg = lua_tostring( L, 1 );
	luaL_traceback( L, L, msg, 1 );
	return 1;
}

} // namespace script
