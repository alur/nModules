#pragma once

#include "../nCoreApi/Version.h"

#include "../Headers/Windows.h"

/// <summary>
/// Creates a message handler for an nModule.
/// </summary>
/// <param name="coreInstance">The instance of nCore.</param>
/// <param name="name">The name of the module.</param>
/// <param name="wndProc">The window procedure.</param>
/// <param name="window">Out, reference to the window handle.</param>
HRESULT CreateMessageHandler(HINSTANCE coreInstance, LPCWSTR name, WNDPROC wndProc, HWND &window);

/// <summary>
/// Handles LiteStep's LM_GETREVID message.
/// </summary>
/// <param name="name">The name of the module.</param>
/// <param name="version">The version of the module.</param>
/// <param name="lParam">The LPARAM sent with LM_GETREVID.</param>
LRESULT HandleGetRevId(LPCWSTR name, VERSION version, LPARAM lParam);

/// <summary>
/// Notifies the user that we failed to connect to nCore.
/// </summary>
/// <param name="hr">The HRESULT returned by nCore::Connect.</param>
void HandleCoreConnectionError(LPCWSTR module, VERSION version, HRESULT hr);
