//-------------------------------------------------------------------------------------------------
// /Utilities/Process.cpp
// The nModules Project
//
// Utilities for dealing with processes.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "Process.h"

#include <Psapi.h>
#include <Shlwapi.h>
#include <strsafe.h>


HRESULT GetProcessName(HWND hWnd, bool fullPath, LPTSTR name, DWORD cchName) {
  DWORD dwProcessID;
  if (GetWindowThreadProcessId(hWnd, &dwProcessID) == 0) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID);
  if (hProc == nullptr) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  if (GetProcessImageFileName(hProc, name, cchName) == FALSE) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  if (!fullPath) {
    return StringCchCopy(name, cchName, PathFindFileName(name));
  }

  return S_OK;
}
