//--------------------------------------------------------------------------------------
// GUID.cpp
// The nModules Project
//
// Utilities for dealing with GUIDs.
//
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include "GUID.h"


BOOL GUIDFromStringA(LPCSTR psz, LPGUID pguid) {
    typedef BOOL (WINAPI* PROCTYPE)(LPCSTR, LPGUID);
    static PROCTYPE proc = nullptr;
    if (proc == nullptr) {
        proc = (PROCTYPE)GetProcAddress(GetModuleHandle(TEXT("Shell32.dll")), (LPCSTR)703);
    }
    return proc(psz, pguid);
}


BOOL GUIDFromStringW(LPCWSTR psz, LPGUID pguid) {
    typedef BOOL (WINAPI* PROCTYPE)(LPCWSTR, LPGUID);
    static PROCTYPE proc = nullptr;
    if (proc == nullptr) {
        proc = (PROCTYPE)GetProcAddress(GetModuleHandle(TEXT("Shell32.dll")), (LPCSTR)704);
    }
    return proc(psz, pguid);
}
