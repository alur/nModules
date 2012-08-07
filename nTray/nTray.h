/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	nTray.h															July, 2012
 *	The nModules Project
 *
 *	General declarations for nTask
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NTASK_H
#define NTASK_H

#include "../nShared/Export.h"

bool CreateLSMsgHandler(HINSTANCE hInst);
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayHandlerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TrayIconHandlerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LoadSettings();

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NTASK_H */
