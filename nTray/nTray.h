/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTray.h
 *  The nModules Project
 *
 *  General declarations for nTask
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Export.h"

bool CreateLSMsgHandler(HINSTANCE hInst);
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayIconHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void LoadSettings();

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);
