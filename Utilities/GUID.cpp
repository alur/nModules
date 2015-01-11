//-------------------------------------------------------------------------------------------------
// /Utilities/GUID.cpp
// The nModules Project
//
// Utilities for dealing with GUIDs.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "GUID.h"


/// <summary>
/// Attempts to parse a string into a GUID.
/// </summary>
/// <param name="guidString">The string to parse.</param>
/// <param name="guid">Out, the parsed GUID.</param>
/// <returns>FALSE if the parsing failed.</param>
BOOL GUIDFromStringA(LPCSTR guidString, LPGUID guid) {
  typedef BOOL(WINAPI *PROCTYPE)(LPCSTR, LPGUID);
  static PROCTYPE proc = nullptr;

  if (proc == nullptr) {
    proc = (PROCTYPE)GetProcAddress(GetModuleHandle(L"Shell32.dll"), (LPCSTR)703);
  }

  if (proc) {
    return proc(guidString, guid);
  }

  return FALSE;
}


/// <summary>
/// Attempts to parse a string into a GUID.
/// </summary>
/// <param name="guidString">The string to parse.</param>
/// <param name="guid">Out, the parsed GUID.</param>
/// <returns>FALSE if the parsing failed.</param>
BOOL GUIDFromStringW(LPCWSTR guidString, LPGUID guid) {
  typedef BOOL(WINAPI *PROCTYPE)(LPCWSTR, LPGUID);
  static PROCTYPE proc = nullptr;

  if (proc == nullptr) {
    proc = (PROCTYPE)GetProcAddress(GetModuleHandle(L"Shell32.dll"), (LPCSTR)704);
  }

  if (proc) {
    return proc(guidString, guid);
  }

  return FALSE;
}
