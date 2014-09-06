//-------------------------------------------------------------------------------------------------
// /nWallpaper/Bangs.cpp
// The nModules Project
//
// Manages nWallpaper specific bang commmands.
//-------------------------------------------------------------------------------------------------
#include "Bangs.h"

#include "../nCoreCom/Core.h"

#include "../nShared/LiteStep.h"

#include "../Utilities/StringUtils.h"
#include "../Utilities/Unordered1To1Map.hpp"

#include <Shlwapi.h>

struct BangItem {
  BangItem(LPCWSTR name, LiteStep::BANGCOMMANDPROC handler) {
    this->name = name;
    this->handler = handler;
  }
  LPCWSTR name;
  LiteStep::BANGCOMMANDPROC handler;
};

static const Unordered1To1Map<LPCWSTR, LPCWSTR, CaseInsensitive::Hash, CaseInsensitive::Hash,
  CaseInsensitive::Equal, CaseInsensitive::Equal> styleMap({
      { L"0", L"Center" },
      { L"2", L"Stretch" },
      { L"6", L"Fit" },
      { L"10", L"Fill" },
      { L"22", L"Span" }
});

static const BangItem bangMap[] = {
  BangItem(L"!SetWallpaper", [] (HWND, LPCWSTR args) {
    WCHAR token1[MAX_PATH], token2[MAX_PATH];
    LPWSTR bufs[] = { token1, token2 };

    LiteStep::CommandTokenize(args, bufs, 1, nullptr);

    // Check if the first token is a valid style.
    LPCWSTR styleValue = styleMap.GetByA(token1, nullptr);
    if (styleValue == nullptr) {
      // Assume that the first token is the wallpaper file
    }

    SHSetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", REG_SZ, token1, (DWORD)wcslen(token1)*sizeof(wchar_t));
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
  }),
  BangItem(L"!SetWallpaperStyle", [] (HWND, LPCWSTR args) {
    WCHAR style[MAX_PATH];
    LPWSTR bufs[] = { style };

    LiteStep::CommandTokenize(args, bufs, _countof(bufs), nullptr);
    LPCWSTR value = styleMap.GetByA(style, nullptr);

    if (value) {
      SHSetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle", REG_SZ, value, (DWORD)wcslen(value)*sizeof(wchar_t));
      SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
    }
  })
};


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
  for (const BangItem &bangItem : bangMap) {
    LiteStep::AddBangCommand(bangItem.name, bangItem.handler);
  }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
  for (const BangItem &bangItem : bangMap) {
    LiteStep::RemoveBangCommand(bangItem.name);
  }
}
