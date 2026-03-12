# Set the command name so that NeoRadiant recognises this file
__commandName__ = "Example"  # should not contain spaces
__commandDisplayName__ = "Nice display name for the menus"  # should not contain spaces


# The actual algorithm called by NeoRadiant is contained in the execute() function
def execute():
    shader = GlobalShaderSystem.getShaderForName("bc_rat")
    print(shader.getName())

    # the "Global*" variables like GlobalShaderSystem are already exposed to this scripts
    # The rest needs to imported from the NeoRadiant module and referred to by the prefix "dr"
    import neoradiant as dr

    GlobalDialogManager.createMessageBox("Example Box", str(e), dr.Dialog.ERROR).run()


# __executeCommand__ evaluates to true after NeoRadiant has successfully initialised
if __executeCommand__:
    execute()
