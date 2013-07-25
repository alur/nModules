//--------------------------------------------------------------------------------------
// Process.h
// The nModules Project
//
// Utilities for dealing with processes.
//
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include "Process.h"
#include <Psapi.h>
#include <strsafe.h>
#include <Shlwapi.h>


HRESULT GetProcessName(HWND hWnd, bool fullPath, LPWSTR name, DWORD cchName) {
    DWORD dwProcessID;
    GetWindowThreadProcessId(hWnd, &dwProcessID);
    HANDLE hProc = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE,
        FALSE, dwProcessID);

    if (hProc == nullptr) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (GetModuleFileNameExW(hProc, nullptr, name, cchName) == FALSE) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (!fullPath) {
        return StringCchCopyW(name, cchName, PathFindFileNameW(name));
    }

    return S_OK;
}
