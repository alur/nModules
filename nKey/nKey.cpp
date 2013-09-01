/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nKey.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nKey module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "nKey.h"
#include "../nShared/ErrorHandler.h"
#include "../nShared/LSModule.hpp"
#include <map>
#include "Version.h"

//
typedef std::map<int, std::tstring> HotkeyMap;
typedef std::map<std::tstring, UINT> VKMap;


// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All hotkey mappings
HotkeyMap gHotKeys;

// Definitions loaded from vk104.txt
VKMap gVKCodes;

// Used for assigning hotkeys.
int gID = 0;

// The LiteStep module class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

//
HWND gWindow;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */)
{
    // Initialize
    if (!gLSModule.Initialize(parent, instance))
    {
        return 1;
    }

    // Load settings
    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */)
{
    // Remove all hotkeys
    for (auto hotkey : gHotKeys)
    {
        UnregisterHotKey(gWindow, hotkey.first);
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
            gWindow = window;
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
/// Loads all settings
/// </summary>
void LoadSettings()
{
    LoadVKeyTable();
    LoadHotKeys();
}


/// <summary>
/// Loads VK definitions
/// </summary>
void LoadVKeyTable()
{
    FILE * file;
    TCHAR path[MAX_PATH], line[256], name[256], code[64];
    LPTSTR tokens[] = { name, code };
    LPTSTR endPtr;
    UINT u;

    LiteStep::GetRCLine(_T("nKeyVKTable"), path, _countof(path), _T(""));
    if (_tfopen_s(&file, path, _T("r")) == 0)
    {
        while (_fgetts(line, sizeof(line), file) != nullptr)
        {
            if (LiteStep::LCTokenize(line, tokens, 2, nullptr) == 2)
            {
                u = _tcstoul(code, &endPtr, 0);

                if (code[0] != '\0' && *endPtr == _T('\0'))
                {
                    gVKCodes[name] = u;
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
void LoadHotKeys()
{
    TCHAR line[MAX_LINE_LENGTH], mods[128], key[128], command[MAX_LINE_LENGTH];
    LPTSTR tokens[] = { mods, key };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, L"*HotKey", line, _countof(line)))
    {
        LiteStep::LCTokenize(line + _countof("*HotKey"), tokens, 2, command);

        // ParseMods expects szMods to be all lowercase.
        _tcslwr_s(mods, _countof(mods));
        AddHotkey(ParseMods(mods), ParseKey(key), command);
    }

    LiteStep::LCClose(f);
}


/// <summary>
/// String -> Mod code
/// </summary>
UINT ParseMods(LPCTSTR modsStr)
{
    UINT mods = 0;
    if (_tcsstr(modsStr, _T("win")) != nullptr) mods |= MOD_WIN;
    if (_tcsstr(modsStr, _T("alt")) != nullptr) mods |= MOD_ALT;
    if (_tcsstr(modsStr, _T("ctrl")) != nullptr) mods |= MOD_CONTROL;
    if (_tcsstr(modsStr, _T("shift")) != nullptr) mods |= MOD_SHIFT;
    if (_tcsstr(modsStr, _T("norepeat")) != nullptr) mods |= MOD_NOREPEAT;
    return mods;
}


/// <summary>
/// String -> Virtual Key Code
/// </summary>
UINT ParseKey(LPCTSTR key)
{
    // If the key is a single character, find that key.
    if (_tcslen(key) == 1)
    {
        return VkKeyScan(key[0]) & 0xFF;
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
/// Adds a hotkey.
/// </summary>
bool AddHotkey(UINT mods, UINT key, LPCTSTR command)
{
    if (mods == -1 || key == -1)
    {
        return false; // Invalid mods or key
    }

    // Register the hotkey
    if (RegisterHotKey(gWindow, gID, mods, key) == FALSE)
    {
        return false; // Failed to register, probably already taken.
    }

    // Add the hotkey definition to the map
    gHotKeys[gID++] = command;

    return true;
}
