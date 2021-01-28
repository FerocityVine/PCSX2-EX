#include "PrecompiledHeader.h"
#include "LuaPS2.h"

using namespace sol;
using namespace std;

vector<wxString> _scriptNames;
vector<LuaPS2*> _loadedScripts;

void ForgetScripts()
{
	_scriptNames.clear();

	for (auto&& _p : _loadedScripts)
		delete(_p);

	_loadedScripts.clear();
}

int LoadScriptFromDir(wxString name, const wxDirName& folderName, const wxString& friendlyName)
{
	Console.WriteLn(Color_Black, L"");
	Console.WriteLn(Color_StrongBlue, L"Initializing LuaEngine v2.0");
	Console.WriteLn(Color_Black, L"");

	u32 _checksum;
	sscanf(name.c_str(), "%x", &_checksum);

	if (!folderName.Exists())
	{
		Console.WriteLn(Color_Red, L"LuaEngine: The \"lua\" folder does not exist! Aborting Initialization...");
		return -1;
	}

	int _interval = 0;
	int _loaded = 0;

	wxString _buffer;
	wxString _luaName = "*" + name + L"*.lua";
	wxDir _dirName(folderName.ToString());

	bool _found = _dirName.GetFirst(&_buffer, L"*", wxDIR_FILES);

	while (_found) 
	{
		if (_buffer.Upper().Matches(_luaName.Upper())) 
		{
			try
			{
				if (_buffer == _scriptNames.at(_interval)) 
				{
					_interval++;
					_found = _dirName.GetNext(&_buffer);
					continue;
				}

				else
				{
					Console.WriteLn(Color_Green, L"LuaEngine: Found Lua Script: '%s'! Initializing...", WX_STR(_buffer));
					_scriptNames.push_back(_buffer);
					_loadedScripts.push_back(new LuaPS2(Path::Combine(_dirName.GetName(), _buffer), _checksum));
					_loaded++;
				}
			}
			
			catch (const out_of_range& e)
			{
				Console.WriteLn(Color_Green, L"LuaEngine: Found Lua Script: '%s'! Initializing...", WX_STR(_buffer));
				_scriptNames.push_back(_buffer);
				_loadedScripts.push_back(new LuaPS2(Path::Combine(_dirName.GetName(), _buffer), _checksum));
				_loaded++;
			}

			_interval++;
		}

		_found = _dirName.GetNext(&_buffer);
	}

	if (_loaded == 0)
		Console.WriteLn(Color_Orange, L"LuaEngine: No Lua scripts were found for this game.", WX_STR(_buffer));

	return _loaded;
}

void ExecuteScript(LUAExecutionTime Input01)
{
	for each (auto _script in _loadedScripts)
	{
		if (!_script->initFunction && !_script->frameFunction)
			return;

		switch (Input01)
		{
			case SPT_ONCE_ON_LOAD:
				_script->initFunction();
				break;
			case SPT_CONTINOUSLY:
				_script->frameFunction();
				break;
		}
	}
}