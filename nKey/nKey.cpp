/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nKey.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nKey module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
static bool AddHotkey(UINT mods, UINT key, LPCWSTR command);
static UINT ParseMods(LPCWSTR mods);
static UINT ParseKey(LPCWSTR key);


// The messages we want from the core
static UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All hotkey mappings
static HotkeyMap gHotKeys;

// Definitions loaded from vk104.txt
static VKMap gVKCodes;

// Used for assigning hotkeys.
static int gID = 0;

// The LiteStep module class
static LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));


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
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR path)
{
    UNREFERENCED_PARAMETER(path);

    // Initialize
    if (!gLSModule.Initialize(parent, instance))
    {
        return 1;
    }

    // Load settings
    LoadVKeyTable();
    LoadHotKeys();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
/// <param name="instance">Handle to this module's instance.</param>
EXPORT_CDECL(void) quitModule(HINSTANCE instance)
{
    UNREFERENCED_PARAMETER(instance);

    // Remove all hotkeys
    for (auto & hotkey : gHotKeys)
    {
        UnregisterHotKey(gLSModule.GetMessageWindow(), hotkey.first);
    }
    
    gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;

    case WM_HOTKEY:
        {
            HotkeyMap::iterator hotkey = gHotKeys.find((int)wParam);
            if (hotkey != gHotKeys.end())
            {
                LiteStep::LSExecute(window, hotkey->second.c_str(), 0);
            }
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Adds a hotkey.
/// </summary>
static bool AddHotkey(UINT mods, UINT key, LPCWSTR command)
{
    if (mods == -1 || key == -1)
    {
        return false; // Invalid mods or key
    }

    // Register the hotkey
    if (RegisterHotKey(gLSModule.GetMessageWindow(), gID, mods, key) == FALSE)
    {
        return false; // Failed to register, probably already taken.
    }

    // Add the hotkey definition to the map
    gHotKeys[gID++] = command;

    return true;
}


/// <summary>
/// Loads VK definitions
/// </summary>
static void LoadVKeyTable()
{
    FILE * file;
    WCHAR path[MAX_PATH], line[256], name[256], code[64];
    LPWSTR tokens[] = { name, code };
    LPWSTR endPtr;
    UINT vkey;

    LiteStep::GetRCLineW(L"nKeyVKTable", path, _countof(path), L"");
    if (_wfopen_s(&file, path, L"r") == 0)
    {
        while (fgetws(line, _countof(line), file) != nullptr)
        {
            if (LiteStep::LCTokenizeW(line, tokens, 2, nullptr) == 2)
            {
                vkey = wcstoul(code, &endPtr, 0);

                if (code[0] != L'\0' && *endPtr == L'\0')
                {
                    gVKCodes[name] = vkey;
                }
            }
        }

        fclose(file);
    }
    else
    {
        // Failed to open the file
    }
}


/// <summary>
/// Reads through the .rc files and load *HotKeys
/// </summary>
static void LoadHotKeys()
{
    WCHAR line[MAX_LINE_LENGTH], mods[128], key[128], command[MAX_LINE_LENGTH];
    LPWSTR tokens[] = { mods, key };
    LPVOID f = LiteStep::LCOpenW(nullptr);

    while (LiteStep::LCReadNextConfigW(f, L"*HotKey", line, _countof(line)))
    {
        LiteStep::LCTokenizeW(line + _countof("*HotKey"), tokens, 2, command);

        // ParseMods expects szMods to be all lowercase.
        _wcslwr_s(mods, _countof(mods));
        AddHotkey(ParseMods(mods), ParseKey(key), command);
    }

    LiteStep::LCClose(f);
}


/// <summary>
/// String -> Virtual Key Code
/// </summary>
static UINT ParseKey(LPCWSTR key)
{
    // If the key is a single character, find that key.
    if (wcslen(key) == 1)
    {
        return VkKeyScanW(key[0]) & 0xFF;
    }
    else
    {
        // Check if it's in our table
        VKMap::const_iterator vk = gVKCodes.find(key);
        if (vk != gVKCodes.end())
        {
            return vk->second;
        }
    }

    // Fail
    return UINT(-1);
}


/// <summary>
/// String -> Mod code
/// </summary>
static UINT ParseMods(LPCWSTR modsStr)
{
    UINT mods = 0;
    if (_tcsstr(modsStr, TEXT("win")) != nullptr) mods |= MOD_WIN;
    if (_tcsstr(modsStr, TEXT("alt")) != nullptr) mods |= MOD_ALT;
    if (_tcsstr(modsStr, TEXT("ctrl")) != nullptr) mods |= MOD_CONTROL;
    if (_tcsstr(modsStr, TEXT("shift")) != nullptr) mods |= MOD_SHIFT;
    if (_tcsstr(modsStr, TEXT("norepeat")) != nullptr) mods |= MOD_NOREPEAT;
    return mods;
}
