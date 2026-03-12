#include "Documentation.h"
#include "registry/registry.h"

#include <wx/utils.h>
#include "imodule.h"

namespace ui
{

void Documentation::showUserGuide(const cmd::ArgumentList&)
{
    const IApplicationContext& ctx = module::GlobalModuleRegistry().getApplicationContext();
    wxLaunchDefaultBrowser(ctx.getHTMLPath() + "manual.html");
}

void Documentation::OpenScriptReference(const cmd::ArgumentList&)
{
    //const IApplicationContext& ctx = module::GlobalModuleRegistry().getApplicationContext();
    wxLaunchDefaultBrowser(registry::getValue<std::string>(RKEY_SCRIPT_REFERENCE_URL));
}

}
