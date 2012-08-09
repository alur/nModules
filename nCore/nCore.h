/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.h                                                       August, 2012
 *  The nModules Project
 *
 *  Functions declarations for nCore.cpp
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NCORE_H
#define NCORE_H

#include "../nShared/Export.h"
#include "../nShared/Versioning.h"

bool CreateMainWindow(HINSTANCE);
LRESULT WINAPI MainProc(HWND, UINT, WPARAM, LPARAM);

EXPORT_CDECL(VERSION) GetCoreVersion();
EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NCORE_H */
