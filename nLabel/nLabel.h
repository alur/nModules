/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.h
 *  The nModules Project
 *
 *  Functions declarations for nLabel.cpp.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Macros.h"

bool CreateLSMsgHandler(HINSTANCE);
LRESULT WINAPI LSMsgHandlerProc(HWND, UINT, WPARAM, LPARAM);
void LoadSettings();

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
