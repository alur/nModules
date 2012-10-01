/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nDesk.h
 *  The nModules Project
 *
 *  Function declarations for nDesk
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/Macros.h"

bool CreateMainWindow(HINSTANCE hInst);
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
