/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.h
 *  The nModules Project
 *
 *  Function declarations for nIcon
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Macros.h"

bool CreateMessageHandlers(HINSTANCE hInst);
LRESULT WINAPI LSMsgHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
