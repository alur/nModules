/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.h
 *  The nModules Project
 *
 *  Functions declarations for nCore.cpp
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Macros.h"
#include "../nShared/Versioning.h"

bool CreateMainWindow(HINSTANCE);
LRESULT WINAPI MainProc(HWND, UINT, WPARAM, LPARAM);

EXPORT_CDECL(VERSION) GetCoreVersion();
EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
