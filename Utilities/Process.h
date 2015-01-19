//-------------------------------------------------------------------------------------------------
// /Utilities/Process.h
// The nModules Project
//
// Utilities for dealing with processes.
//-------------------------------------------------------------------------------------------------
#pragma once

/// <summary>
/// Retrives the name of a process, based on a window handle.
/// </summary>
/// <param name="hWnd">The window handle for which to get the process name.</param>
/// <param name="fullPath">
/// If true, the full path to the process is returned. If false, only the file name
/// portion is returned.
/// </param>
/// <param name="name">Buffer that receives the process name.</param>
/// <param name="cchName">The size of the name buffer.</param>
HRESULT GetProcessName(HWND hwndProcess, bool fullPath, LPTSTR name, DWORD cchName);
