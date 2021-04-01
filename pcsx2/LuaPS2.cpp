#include "PrecompiledHeader.h"

#include "LuaPS2.h"
#include "AppConfig.h"

u32 LuaPS2::_checksum;
wxString LuaPS2::_loadPath;
wxString LuaPS2::_savePath;

bool LuaPS2::_isSaving = false;

LuaPS2::LuaPS2(wxString Input01, u32 Input02)
{
	luaState.open_libraries
	(
		lib::base,
		lib::package,
		lib::coroutine,
		lib::string,
		lib::os,
		lib::math,
		lib::table,
		lib::io,
		lib::bit32,
		lib::utf8
	);

	SetFunctions();

	_loadPath = Path::Combine(Path::GetDirectory(Input01), "io_load");

	luaState["package"]["path"] = Path::Combine(Path::GetDirectory(Input01), "io_packages/").ToStdString() + "?.lua";
	luaState["package"]["cpath"] = Path::Combine(Path::GetDirectory(Input01), "io_packages/").ToStdString() + "?.dll";

	luaState["LOAD_PATH"] = _loadPath.ToStdString();
	luaState["SCRIPT_PATH"] = Path::GetDirectory(Input01).ToStdString();
	luaState["CHEATS_PATH"] = GetCheatsFolder().ToString().ToStdString();

	luaState["ENGINE_VERSION"] = 2.5F;
	luaState["ENGINE_TYPE"] = "ENGINE";
	luaState["GAME_ID"] = Input02;

	auto _result = luaState.script_file(Input01.ToStdString());

	bootFunction = luaState["_OnBoot"];
	initFunction = luaState["_OnInit"];
	frameFunction = luaState["_OnFrame"];

	if (!bootFunction)
		Console.WriteLn(Color_Red, L"LuaEngine: The \"_OnBoot\" function either has errors or does not exist.");

	if (!initFunction)
		Console.WriteLn(Color_Red, L"LuaEngine: The \"_OnInit\" function either has errors or does not exist.");

	if (!frameFunction)
		Console.WriteLn(Color_Red, L"LuaEngine: The \"_OnFrame\" function either has errors or does not exist.");

	if (!initFunction && !frameFunction && !bootFunction)
	{
		Console.WriteLn(Color_Red, L"\nLuaEngine: None of the needed functions exist or all of them have errors.");
		Console.WriteLn(Color_Red, L"LuaEngine: Initialization of this script cannot continue...\n");
		return;
	}

	Console.WriteLn(Color_Green, L"LuaEngine: Initialization Successful!");
	Console.WriteLn(Color_Black, L"");

	if (initFunction)
		initFunction();
}


void LuaPS2::SetFunctions()
{
	// Readers
	luaState.set_function("ReadByte",    Read_UInt08);
	luaState.set_function("ReadShort",   Read_UInt16);
	luaState.set_function("ReadInt",     Read_UInt32);
	luaState.set_function("ReadLong",    Read_UInt64);
	luaState.set_function("ReadBoolean", Read_Boolean);
	luaState.set_function("ReadFloat",   Read_Single);
	luaState.set_function("ReadString",  Read_String);
	luaState.set_function("ReadArray",   Read_UInt08_Array);

	// Calculators
	luaState.set_function("GetPointer", Calculate_Pointer);

	// IO Operations
	luaState.set_function("ReadFile", sol::overload(File_Read, File_ReadRegion));

	// Writers
	luaState.set_function("WriteByte",    Write_UInt08);
	luaState.set_function("WriteShort",   Write_UInt16);
	luaState.set_function("WriteInt",     Write_UInt32);
	luaState.set_function("WriteLong",    Write_UInt64);
	luaState.set_function("WriteBoolean", Write_Boolean);
	luaState.set_function("WriteFloat",   Write_Single);
	luaState.set_function("WriteString",  Write_String);
	luaState.set_function("WriteArray",   Write_UInt08_Array);

	// Misc
	luaState.set_function("ConsolePrint", PCSX2Print);
}

