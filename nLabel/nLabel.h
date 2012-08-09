/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.h                                                        July, 2012
 *  The nModules Project
 *
 *  Functions declarations for nLabel.cpp.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef NLABEL_H
#define NLABEL_H

#include "../nShared/Export.h"

bool CreateLSMsgHandler(HINSTANCE);
LRESULT WINAPI LSMsgHandlerProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI LabelHandlerProc(HWND, UINT, WPARAM, LPARAM);
void LoadSettings();

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

#endif /* NLABEL_H */
