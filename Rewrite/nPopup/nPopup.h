#pragma once

#include "PopupLevel.h"
#include "PopupLineType.h"

/// <summary>
/// Adds a new root-level popup.
/// </summary>
static void AddPopup(LPCWSTR bang, Popup *popup);

/// <summary>
/// Loads a popup.
/// </summary>
/// <return>Returns false when all lines have been read.</return>
static bool LoadPopup(LPVOID f, PopupLevel level, Popup *&popup, LPWSTR bang, size_t cchBang,
    LPCWSTR parentPrefix);

/// <summary>
/// Parses a *Popup line.
/// </summary>
static PopupLineType ParseLine(LPCTSTR line, LPTSTR title, UINT cchTitle, LPTSTR command,
  UINT cchCommand, LPTSTR icon, UINT cchIcon, LPTSTR prefix, UINT cchPrefix);