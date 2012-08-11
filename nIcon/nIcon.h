/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.h                                                         July, 2012
 *  The nModules Project
 *
 *  Function declarations for nIcon
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NICON_H
#define NICON_H

#include "../nShared/Export.h"

bool CreateMessageHandlers(HINSTANCE hInst);
LRESULT WINAPI LSMsgHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI GroupHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NICON_H */
