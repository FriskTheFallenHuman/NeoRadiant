#include "DialogInterface.h"
#include "../LuaHelper.h"
#include "ui/idialogmanager.h"

namespace script
{

constexpr const char* META_DIALOG = "NeoRadiant.Dialog";

void DialogInterface::registerInterface( lua_State* L )
{
	// IDialog wrapper metatable
	static luaL_Reg dialogMethods[] =
		{ { "setTitle",
			[](lua_State* L)->int {
				lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->setTitle( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "addLabel",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->addLabel( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "addEntryBox",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->addEntryBox( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "addPathEntry",
			[](lua_State* L)->int {
				bool browse			= lua_toboolean( L, 3 ) != 0;
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->addPathEntry( lua_checkstdstring( L, 2 ), browse ) );
				return 1;
			} },
		{ "addSpinButton",
			[](lua_State* L)->int {
				lua_pushinteger( L,
					( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )
						->addSpinButton( lua_checkstdstring( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ), luaL_checknumber( L, 5 ), ( unsigned int )luaL_optinteger( L, 6, 0 ) ) );
				return 1;
			} },
		{ "addCheckbox",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->addCheckbox( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "addComboBox",
			[](lua_State* L)->int {
				luaL_checktype( L, 3, LUA_TTABLE );
				ui::IDialog::ComboBoxOptions opts;
				lua_Integer n				= ( lua_Integer )lua_rawlen( L, 3 );
				for( lua_Integer i = 1; i <= n; ++i ) {
					lua_rawgeti( L, 3, i );
					opts.push_back( lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "" );
					lua_pop( L, 1 );
				}
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->addComboBox( lua_checkstdstring( L, 2 ), opts ) );
				return 1;
			} },
		{ "getElementValue",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->getElementValue( ( ui::IDialog::Handle )luaL_checkinteger( L, 2 ) ) );
				return 1;
			} },
		{ "setElementValue",
			[](lua_State* L)->int {
				lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->setElementValue( ( ui::IDialog::Handle )luaL_checkinteger( L, 2 ), lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ "run",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<ui::IDialog>( L, 1, META_DIALOG )->run() );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_DIALOG, dialogMethods );

	// GlobalDialogManager
	static luaL_Reg mgr[] =
		{ { "createDialog",
			[](lua_State* L)->int {
				auto dlg = GlobalDialogManager().createDialog( lua_checkstdstring( L, 2 ) );
				if( !dlg ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, dlg.get(), META_DIALOG );
				return 1;
			} },
		{ "createMessageBox",
			[](lua_State* L)->int {
				std::string title = lua_checkstdstring( L, 2 );
				std::string text  = lua_checkstdstring( L, 3 );
				auto msgType = static_cast<ui::IDialog::MessageType>( luaL_checkinteger( L, 4 ) );
				auto dlg	  = GlobalDialogManager().createMessageBox( title, text, msgType );
				if( !dlg ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, dlg.get(), META_DIALOG );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.DialogManager", mgr );
	lua_setglobal_object( L, "GlobalDialogManager", this, "NeoRadiant.DialogManager" );

	// Dialog table
	lua_newtable( L );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::MESSAGE_CONFIRM );
	lua_setfield( L, -2, "CONFIRM" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::MESSAGE_ASK );
	lua_setfield( L, -2, "ASK" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::MESSAGE_WARNING );
	lua_setfield( L, -2, "WARNING" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::MESSAGE_ERROR );
	lua_setfield( L, -2, "ERROR" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::MESSAGE_YESNOCANCEL );
	lua_setfield( L, -2, "YESNOCANCEL" );

	// Result constants
	lua_pushinteger( L, ( lua_Integer )ui::IDialog::RESULT_OK );
	lua_setfield( L, -2, "OK" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::RESULT_CANCELLED );
	lua_setfield( L, -2, "CANCELLED" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::RESULT_YES );
	lua_setfield( L, -2, "YES" );

	lua_pushinteger( L, ( lua_Integer )ui::IDialog::RESULT_NO );
	lua_setfield( L, -2, "NO" );

	lua_setglobal( L, "Dialog" );
}

} // namespace script
