#include "SoundManager.h"

#include "imodule.h"

extern "C" void NEORADIANT_DLLEXPORT RegisterModule(IModuleRegistry& registry)
{
	module::performDefaultInitialisation(registry);

	registry.registerModule(std::make_shared<sound::SoundManager>());
}
