/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	nLabel.h														July, 2012
 *	The nModules Project
 *
 *	Functions declarations for nLabel.cpp.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NLABEL_H
#define NLABEL_H

#include "../nShared/Export.h"

bool CreateLSMsgHandler(HINSTANCE);
LRESULT WINAPI LSMsgHandlerProc(HWND, UINT, WPARAM, LPARAM);
void LoadSettings();
void LoadHotKeys();
void LoadVKeyTable();
bool AddHotkey(UINT mods, UINT key, LPCSTR pszCommand);
UINT ParseMods(LPCSTR szMods);
UINT ParseKey(LPCSTR szKey);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NLABEL_H */
