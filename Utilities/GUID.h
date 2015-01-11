//-------------------------------------------------------------------------------------------------
// /Utilities/GUID.h
// The nModules Project
//
// Utilities for dealing with GUIDs.
//-------------------------------------------------------------------------------------------------
#pragma once

BOOL GUIDFromStringA(LPCSTR guidString, LPGUID guid);
BOOL GUIDFromStringW(LPCWSTR guidString, LPGUID guid);

#if defined(_UNICODE)
#   define GUIDFromString GUIDFromStringW
#else
#   define GUIDFromString GUIDFromStringA
#endif
