/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nDesk.h                                                         July, 2012
 *  The nModules Project
 *
 *  Function declarations for nDesk
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NDESK_H
#define NDESK_H

#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/Export.h"

bool CreateMainWindow(HINSTANCE hInst);
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NDESK_H */
