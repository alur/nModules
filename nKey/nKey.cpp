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


using std::map;

// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All hotkey mappings
map<int, LPCTSTR> g_hotKeys;

// Definitions loaded from vk104.txt
map<LPCTSTR, UINT> g_vkCodes;

// Used for assigning hotkeys.
int g_id = 0;

// The LiteStep module class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

//
HWND g_window;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */) {
    // Initialize    
    if (!gLSModule.Initialize(parent, instance)) {
        return 1;
    }

    // Load settings
    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all hotkeys
    for (auto hotkey : g_hotKeys) {
        UnregisterHotKey(g_window, hotkey.first);
        free(const_cast<LPTSTR>(hotkey.second));
    }
    g_hotKeys.clear();

    // Clear g_vkCodes
    for (auto code : g_vkCodes) {
        free(const_cast<LPTSTR>(code.first));
    }
    g_vkCodes.clear();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="messageThe type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            g_window = window;
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
            map<int, LPCTSTR>::iterator iter = g_hotKeys.find((int)wParam);
            if (iter != g_hotKeys.end()) {
                LiteStep::LSExecute(window, iter->second, 0);
            }
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Loads all settings
/// </summary>
void LoadSettings() {
    LoadVKeyTable();
    LoadHotKeys();
}


/// <summary>
/// Loads VK definitions
/// </summary>
void LoadVKeyTable() {
    FILE * file;
    TCHAR path[MAX_PATH], line[256], szName[256], szCode[64];
    LPTSTR tokens[] = { szName, szCode };
    TCHAR * endPtr;
    UINT u;

    LiteStep::GetRCLine(_T("nKeyVKTable"), path, _countof(path), _T(""));
    if (_tfopen_s(&file, path, _T("r")) == 0) {
        while (_fgetts(line, sizeof(line), file) != nullptr) {
            if (LiteStep::LCTokenize(line, tokens, 2, nullptr) == 2) {
                u = _tcstoul(szCode, &endPtr, 0);

                if (szCode[0] != '\0' && *endPtr == '\0') {
                    // Store the name in the heap
                    size_t size = _tcslen(szName)+1;
                    LPTSTR pszHeap = (LPTSTR)malloc(size);
                    _tcscpy_s(pszHeap, size, szName);

                    // Insert it into the map
                    g_vkCodes.insert(g_vkCodes.begin(), std::pair<LPCTSTR, UINT>(pszHeap, u));
                }
            }
        }

        fclose(file);
    }
    else {
        // Failed to open the file
    }
}


/// <summary>
/// Reads through the .rc files and load *HotKeys
/// </summary>
void LoadHotKeys() {
    TCHAR szLine[MAX_LINE_LENGTH], szMods[128], szKey[128], szCommand[MAX_LINE_LENGTH];
    LPTSTR szTokens[] = { szMods, szKey };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, L"*HotKey", szLine, _countof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*HotKey")+1, szTokens, 2, szCommand);
        // ParseMods expects szMods to be all lowercase.
        _tcslwr_s(szMods, _countof(szMods));
        AddHotkey(ParseMods(szMods), ParseKey(szKey), szCommand);
    }

    LiteStep::LCClose(f);
}


/// <summary>
/// String -> Mod code
/// </summary>
UINT ParseMods(LPCTSTR szMods) {
    UINT mods = 0;
    if (_tcsstr(szMods, _T("win")) != NULL) mods |= MOD_WIN;
    if (_tcsstr(szMods, _T("alt")) != NULL) mods |= MOD_ALT;
    if (_tcsstr(szMods, _T("ctrl")) != NULL) mods |= MOD_CONTROL;
    if (_tcsstr(szMods, _T("shift")) != NULL) mods |= MOD_SHIFT;
    return mods;
}


/// <summary>
/// String -> Virtual Key Code
/// </summary>
UINT ParseKey(LPCTSTR szKey) {
    // If the key is a single character, find that key.
    if (_tcslen(szKey) == 1) {
        return VkKeyScan(szKey[0]) & 0xFF;
    }
    else {
        // Check if it's in our table
        for (map<LPCTSTR, UINT>::const_iterator iter = g_vkCodes.begin(); iter != g_vkCodes.end(); iter++) {
            if (_tcscmp(iter->first, szKey) == 0) {
                return iter->second;
            }
        }
    }
    // Fail
    return UINT(-1);
}


/// <summary>
/// Adds a hotkey.
/// </summary>
bool AddHotkey(UINT mods, UINT key, LPCTSTR pszCommand) {
    // Invalid mods or key
    if (mods == -1 || key == -1) return false;

    // Store the command in the heap
    size_t size = _tcslen(pszCommand)+1;
    LPTSTR pszCommandHeap = (LPTSTR)malloc(size);
    _tcscpy_s(pszCommandHeap, size, pszCommand);

    // Add the hotkey definition
    map<int, LPCTSTR>::iterator it = g_hotKeys.insert(g_hotKeys.begin(), std::pair<int, LPCTSTR>(g_id, pszCommandHeap));

    // Register the hotkey
    if (RegisterHotKey(g_window, g_id, mods, key) == FALSE) {
        g_hotKeys.erase(it);
        return false; // Failed to register, probably already taken.
    }

    // Increment the global hotkey id
    g_id++;
    return true;
}
