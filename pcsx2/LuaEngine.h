#ifndef LUA_HEAD
#define LUA_HEAD

#include "Pcsx2Types.h"

#include "Memory.h"
#include "AppSaveStates.h"

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>
#include <wx/textfile.h>
#include <wx/dir.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>

enum LUAExecutionTime
{
	SPT_ONCE_ON_LOAD = 0,
	SPT_CONTINOUSLY = 1,

	_SPT_END_MARKER
};

extern void ForgetScripts();
extern void ExecuteScript(LUAExecutionTime Input01);
extern int LoadScriptFromDir(wxString Input01, const wxDirName &Input02, const wxString &Input03);

#endif