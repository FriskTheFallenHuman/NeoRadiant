-- dialogtest.lua
-- Demonstrates the dialog API

local dialog = GlobalDialogManager:createDialog("Test")

dialog:addLabel("Testlabel")

local entryHandle  = dialog:addEntryBox("Entry")
local spinHandle   = dialog:addSpinButton("Spinner", 0, 10, 0.5, 1)

-- Combo box options as a plain Lua table of strings
local options = { "Test1", "Test2" }
dialog:addComboBox("Test", options)

dialog:addCheckbox("TestCheckbox")

if dialog:run() == Dialog.OK then
    local result = GlobalDialogManager:createMessageBox(
        "Result",
        "User pressed OK, entry is: <b>" .. dialog:getElementValue(entryHandle) .. "</b>",
        Dialog.CONFIRM)
    result:setTitle("Result Popup Message")
    result:run()
end
