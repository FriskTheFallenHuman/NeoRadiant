#include "itextstream.h"

#include "ScriptingSystem.h"
#include "debugging/debugging.h"

/**
 * greebo: This is the module entry point which the main binary will look for.
 * The symbol RegisterModule is called with the singleton ModuleRegistry as argument.
 */
extern "C" void NEORADIANT_DLLEXPORT RegisterModule( IModuleRegistry& registry )
{
	module::performDefaultInitialisation( registry );

	registry.registerModule( std::make_shared<script::ScriptingSystem>() );
}
