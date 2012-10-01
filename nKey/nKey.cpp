/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nKey.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nKey module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "nKey.h"
#include "../nShared/Error.h"
#include "../nShared/LSModule.hpp"
#include <map>

using std::map;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// All hotkey mappings
map<int, LPCSTR> g_hotKeys;

// Definitions loaded from vk104.txt
map<LPCSTR, UINT> g_vkCodes;

// Used for assigning hotkeys.
int g_id = 0;

// The LiteStep module class
LSModule* g_LSModule;

// The window classes we want to register
LPCSTR g_windowClasses[] = { NULL };


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE instance, LPCSTR /* szPath */) {
    // Initialize
    g_LSModule = new LSModule("nKey", "Alurcard2", MAKE_VERSION(1,0,0,0), instance, g_lsMessages);
    
    if (!g_LSModule->Initialize(g_windowClasses)) {
        delete g_LSModule;
        return 1;
    }

    // Load settings
    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Remove all hotkeys
    for (map<int, LPCSTR>::const_iterator iter = g_hotKeys.begin(); iter != g_hotKeys.end(); iter++) {
        UnregisterHotKey(g_LSModule->GetMessageWindow(), iter->first);
        free((LPVOID)iter->second);
    }
    g_hotKeys.clear();

    // Clear g_vkCodes
    for (map<LPCSTR, UINT>::const_iterator iter = g_vkCodes.begin(); iter != g_vkCodes.end(); iter++) {
        free((LPVOID)iter->first);
    }
    g_vkCodes.clear();

    // Deinitalize
    if (g_LSModule) {
        delete g_LSModule;
    }
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case LM_REFRESH: {
            return 0;
        }
        case WM_HOTKEY: {
            map<int, LPCSTR>::iterator iter = g_hotKeys.find((int)wParam);
            if (iter != g_hotKeys.end()) {
                LSExecute(hWnd, iter->second, 0);
            }
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
    char path[MAX_PATH], line[256], szName[256], szCode[64];
    LPSTR tokens[] = { szName, szCode };
    char * endPtr;
    UINT u;

    GetRCLine("nKeyVKTable", path, sizeof(path), "");
    if (fopen_s(&file, path, "r") == 0) {
        while (fgets(line, sizeof(line), file) != NULL) {
            if (LCTokenize(line, tokens, 2, NULL) == 2) {
                u = strtoul(szCode, &endPtr, 0);

                if (szCode[0] != '\0' && *endPtr == '\0') {
                    // Store the name in the heap
                    size_t size = strlen(szName)+1;
                    LPSTR pszHeap = (LPSTR)malloc(size);
                    strcpy_s(pszHeap, size, szName);

                    // Insert it into the map
                    g_vkCodes.insert(g_vkCodes.begin(), std::pair<LPCSTR, UINT>(pszHeap, u));
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
    char szLine[MAX_LINE_LENGTH], szMods[128], szKey[128], szCommand[MAX_LINE_LENGTH];
    LPSTR szTokens[] = {szMods, szKey};
    LPVOID f = LCOpen(NULL);

    while (LCReadNextConfig(f, "*HotKey", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*HotKey")+1, szTokens, 2, szCommand);
        // ParseMods expects szMods to be all lowercase.
        _strlwr_s(szMods, sizeof(szMods));
        AddHotkey(ParseMods(szMods), ParseKey(szKey), szCommand);
    }

    LCClose(f);
}


/// <summary>
/// String -> Mod code
/// </summary>
UINT ParseMods(LPCSTR szMods) {
    UINT mods = 0;
    if (strstr(szMods, "win") != NULL) mods |= MOD_WIN;
    if (strstr(szMods, "alt") != NULL) mods |= MOD_ALT;
    if (strstr(szMods, "ctrl") != NULL) mods |= MOD_CONTROL;
    if (strstr(szMods, "shift") != NULL) mods |= MOD_SHIFT;
    return mods;
}


/// <summary>
/// String -> Virtual Key Code
/// </summary>
UINT ParseKey(LPCSTR szKey) {
    // If the key is a single character, find that key.
    if (strlen(szKey) == 1) {
        return VkKeyScan(szKey[0]) & 0xFF;
    }
    else {
        // Check if it's in our table
        for (map<LPCSTR, UINT>::const_iterator iter = g_vkCodes.begin(); iter != g_vkCodes.end(); iter++) {
            if (strcmp(iter->first, szKey) == 0) {
                return iter->second;
            }
        }
    }
    // Fail
    return (UINT)-1;
}


/// <summary>
/// Adds a hotkey.
/// </summary>
bool AddHotkey(UINT mods, UINT key, LPCSTR pszCommand) {
    // Invalid mods or key
    if (mods == -1 || key == -1) return false;

    // Store the command in the heap
    size_t size = strlen(pszCommand)+1;
    LPSTR pszCommandHeap = (LPSTR)malloc(size);
    strcpy_s(pszCommandHeap, size, pszCommand);

    // Add the hotkey definition
    map<int, LPCSTR>::iterator it = g_hotKeys.insert(g_hotKeys.begin(), std::pair<int, LPCSTR>(g_id, pszCommandHeap));

    // Register the hotkey
    if (RegisterHotKey(g_LSModule->GetMessageWindow(), g_id, mods, key) == FALSE) {
        g_hotKeys.erase(it);
        return false; // Failed to register, probably already taken.
    }

    // Increment the global hotkey id
    g_id++;
    return true;
}
