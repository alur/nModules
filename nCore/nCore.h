/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.h
 *  The nModules Project
 *
 *  Functions declarations for nCore.cpp
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Macros.h"
#include "../Utilities/Versioning.h"

bool CreateMainWindow(HINSTANCE);
LRESULT WINAPI MainProc(HWND, UINT, WPARAM, LPARAM);

EXPORT_CDECL(VERSION) GetCoreVersion();
EXPORT_CDECL(int) initModuleW(HWND, HINSTANCE, LPCWSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
