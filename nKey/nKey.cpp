//-------------------------------------------------------------------------------------------------
// /nKey/nKey.cpp
// The nModules Project
//
// nKey entry points.
//-------------------------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include "Version.h"

#include "../nShared/ErrorHandler.h"
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/StringUtils.h"

#include <unordered_map>
#include <strsafe.h>

typedef std::unordered_map<int, std::wstring> HotkeyMap;
typedef StringKeyedMaps<std::wstring, UINT, CaseSensitive>::UnorderedMap VKMap;

static void LoadSettings();
static void LoadHotKeys();
static void LoadVKeyTable();
static std::pair<bool, LPCWSTR> AddHotkey(UINT mods, UINT key, LPCWSTR command);
static UINT ParseMods(LPCWSTR mods);
static UINT ParseKey(LPCWSTR key);

// The messages we want from the core
static UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All hotkey mappings
static HotkeyMap gHotKeys;

// Definitions loaded from vk104.txt
static VKMap gVKCodes;

// Used for assigning hotkeys.
static int gId = 0;

// The LiteStep module class
static LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));


static void Load() {
  LoadVKeyTable();
  LoadHotKeys();
}


static void Unload() {
  for (auto &hotkey : gHotKeys) {
    UnregisterHotKey(gLSModule.GetMessageWindow(), hotkey.first);
  }
  gHotKeys.clear();
  gVKCodes.clear();
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
/// <param name="parent"></param>
/// <param name="instance">Handle to this module's instance.</param>
/// <param name="path">Path to the LiteStep directory.</param>
/// <returns>0 on success, non-zero on error.</returns>
/// <remarks>
/// If this function returns non-zero, the module will be unloaded immediately, without
/// going through quitModule.
/// </remarks>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */) {
  if (!gLSModule.Initialize(parent, instance)) {
    return 1;
  }
  Load();
  return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
/// <param name="instance">Handle to this module's instance.</param>
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  Unload();
  gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  using LiteStep::GetLitestepWnd;
  switch(message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case LM_REFRESH:
    Unload();
    Load();
    return 0;

  case WM_HOTKEY:
    HotkeyMap::iterator hotkey = gHotKeys.find((int)wParam);
    if (hotkey != gHotKeys.end()) {
      LiteStep::LSExecute(window, hotkey->second.c_str(), 0);
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Adds a hotkey.
/// </summary>
static std::pair<bool, LPCWSTR> AddHotkey(UINT mods, UINT key, LPCWSTR command) {
  if (mods == -1 || key == -1) {
    return std::make_pair(false, L"Invalid modifiers or key.");
  }

  // Register the hotkey
  if (RegisterHotKey(gLSModule.GetMessageWindow(), gId, mods, key) == FALSE) {
    return std::make_pair(false, L"Failed to register the hotkey. Probably already taken.");
  }

  // Add the hotkey definition to the map
  gHotKeys[gId++] = command;

  return std::make_pair(true, nullptr);
}


/// <summary>
/// Loads VK definitions
/// </summary>
static void LoadVKeyTable() {
  FILE *file;
  WCHAR path[MAX_PATH], line[256], name[256], code[64];
  LPWSTR tokens[] = { name, code };
  LPWSTR endPtr;
  UINT vkey;

  if (LiteStep::GetRCLine(L"nKeyVKTable", path, _countof(path), L"") != 0) {
    errno_t result = _wfopen_s(&file, path, L"r");
    if (result == 0) {
      while (fgetws(line, _countof(line), file) != nullptr) {
        if (line[0] == ';') {
          continue;
        }
        if (LiteStep::LCTokenize(line, tokens, 2, nullptr) == 2) {
          vkey = wcstoul(code, &endPtr, 0);
          if (*code != L'\0' && *endPtr == L'\0') {
            gVKCodes[name] = vkey;
          } else {
            ErrorHandler::Error(ErrorHandler::Level::Warning,
              L"Invalid line in nKeyVKTable.\n%ls", line);
          }
        } else if (line[0] != L'\n') {
          ErrorHandler::Error(ErrorHandler::Level::Warning,
            L"Invalid line in nKeyVKTable.\n%ls", line);
        }
      }
      fclose(file);
    } else {
      ErrorHandler::Error(ErrorHandler::Level::Warning,
        L"Unable to open nKeyVKTable, %ls.\n%ls", file, _wcserror(result));
    }
  }
}


/// <summary>
/// Reads through the .rc files and load *HotKeys
/// </summary>
static void LoadHotKeys() {
  WCHAR line[MAX_LINE_LENGTH], mods[128], key[128], command[MAX_LINE_LENGTH];
  LPWSTR tokens[] = { mods, key };
  LPVOID f = LiteStep::LCOpenW(nullptr);

  while (LiteStep::LCReadNextConfigW(f, L"*HotKey", line, _countof(line))) {
    LiteStep::LCTokenizeW(line + _countof("*HotKey"), tokens, 2, command);

    // ParseMods expects szMods to be all lowercase.
    _wcslwr_s(mods, _countof(mods));
    std::pair<bool, LPCWSTR> result = AddHotkey(ParseMods(mods), ParseKey(key), command);
    if (!result.first) {
      ErrorHandler::Error(ErrorHandler::Level::Warning,
        L"Error while registering hotkey %ls %ls.\n%ls", mods, key, result.second);
    }
  }

  LiteStep::LCClose(f);
}


/// <summary>
/// String -> Virtual Key Code
/// </summary>
static UINT ParseKey(LPCWSTR key) {
  // If the key is a single character, find that key.
  if (wcslen(key) == 1) {
    return VkKeyScanW(key[0]) & 0xFF;
  } else {
    // Check if it's in our table
    VKMap::const_iterator vk = gVKCodes.find(key);
    if (vk != gVKCodes.end()) {
      return vk->second;
    }
  }

  // Fail
  return UINT(-1);
}


/// <summary>
/// String -> Mod code
/// </summary>
static UINT ParseMods(LPCWSTR modsStr) {
  UINT mods = 0;
  if (wcsstr(modsStr, L"win") != nullptr) mods |= MOD_WIN;
  if (wcsstr(modsStr, L"alt") != nullptr) mods |= MOD_ALT;
  if (wcsstr(modsStr, L"ctrl") != nullptr) mods |= MOD_CONTROL;
  if (wcsstr(modsStr, L"shift") != nullptr) mods |= MOD_SHIFT;
  if (wcsstr(modsStr, L"norepeat") != nullptr) mods |= MOD_NOREPEAT;
  return mods;
}
