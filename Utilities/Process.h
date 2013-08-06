/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Process.h
 *  The nModules Project
 *
 *  Utilities for dealing with processes.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

HRESULT GetProcessName(HWND hwndProcess, bool fullPath, LPTSTR name, DWORD cchName);
