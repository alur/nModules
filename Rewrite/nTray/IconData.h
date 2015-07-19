#pragma once

#include "../Headers/lsapi.h"

struct IconData {
  IconData() : guid(GUID_NULL), window(nullptr), icon(nullptr), id(0), version(0),
    callbackMessage(0), flags(0), processId(0)
  {
    tip[0] = L'\0';
  }

  GUID guid;
  HWND window;
  HICON icon;
  UINT id;
  UINT version;
  UINT callbackMessage;
  DWORD flags;
  DWORD processId;
  WCHAR tip[TRAY_MAX_TIP_LENGTH];
};
