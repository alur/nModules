//-------------------------------------------------------------------------------------------------
// /Utilities/Error.h
// The nModules Project
//
// Functions for dealing with errors.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Headers/Windows.h"

inline HRESULT HrGetLastError() {
  return HRESULT_FROM_WIN32(GetLastError());
}

/// <summary>
/// Retrives a description of a HRESULT error code.
/// </summary>
HRESULT DescriptionFromHR(HRESULT hr, LPTSTR buf, size_t cchBuf);
