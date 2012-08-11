/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTray.h                                                         July, 2012
 *  The nModules Project
 *
 *  General declarations for nTask
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NTRAY_H
#define NTRAY_H

#include "../nShared/Export.h"

bool CreateLSMsgHandler(HINSTANCE hInst);
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayIconHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void LoadSettings();

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NTRAY_H */
