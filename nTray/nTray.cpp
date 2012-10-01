/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTask.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "nTray.h"
#include "../nShared/nShared.h"
#include "../nCoreCom/Core.h"
#include <map>
#include "../nShared/Factories.h"
#include "Tray.hpp"
#include "TrayManager.h"

using std::map;

const VERSION g_minCoreVersion      = MAKE_VERSION(0,2,0,0);
const VERSION g_version             = MAKE_VERSION(0,2,0,0);
LPCSTR g_szAppName                  = "nTray";
LPCSTR g_szMsgHandler               = "LSnTrayMsgHandler";
LPCSTR g_szTrayHandler              = "LSnTrayHandler";
LPCSTR g_szTrayIconHandler          = "LSnTrayIconHandler";
LPCSTR g_szAuthor                   = "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, LM_SYSTRAY, LM_SYSTRAYINFOEVENT, 0 };

// Handle to the message handler window
HWND g_hWndMsgHandler;

// Handle to the tray notify window
HWND g_hWndTrayNotify;

// This instance.
HINSTANCE g_hInstance;

// All the trays we currently have loaded
map<LPCSTR, Tray*> g_Trays;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE hDllInstance, LPCSTR /* szPath */) {
    g_hInstance = hDllInstance;

    // Initalize communication with the core
    switch (nCore::Init(g_minCoreVersion)) {
    case S_OK:
        break;
    default:
        ErrorMessage(E_LVL_ERROR, "There was a problem connecting to nCore!");
        return 1;
    }

    // Initialize
    if (!CreateLSMsgHandler(hDllInstance)) return 1;

    // Load settings
    LoadSettings();

    // Let the core know that we want the system tray icons
    g_hWndTrayNotify = (HWND)SendMessage(GetLitestepWnd(), LM_SYSTRAYREADY, NULL, NULL);

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Remove all trays
    for (map<LPCSTR, Tray*>::const_iterator iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
        delete iter->second;
    }
    g_Trays.clear();

    // Deinitalize
    if (g_hWndMsgHandler) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hWndMsgHandler, (LPARAM)g_lsMessages);
        DestroyWindow(g_hWndMsgHandler);
    }

    TrayManager::Stop();

    UnregisterClass(g_szMsgHandler, hDllInstance);
    UnregisterClass(g_szTrayHandler, hDllInstance);
    UnregisterClass(g_szTrayIconHandler, hDllInstance);

    Factories::Release();
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hDllInstance">The instance to attach this message handler to.</param>
bool CreateLSMsgHandler(HINSTANCE hDllInstance) {
    WNDCLASSEX wc;

    // Register the LiteStep message handler window class.
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MainProc;
    wc.hInstance = hDllInstance;
    wc.lpszClassName = g_szMsgHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTray's msg window class!"));
        return false;
    }

    if (!DrawableWindow::RegisterWindowClass(g_szTrayHandler, hDllInstance)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTray's tray window class!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        return false;
    }

    if (!DrawableWindow::RegisterWindowClass(g_szTrayIconHandler, hDllInstance)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTray's icon window class!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        UnregisterClass(g_szTrayHandler, hDllInstance);
        return false;
    }

    // Create the LiteStep message handler window
    g_hWndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMsgHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!g_hWndMsgHandler) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to create nTray's message handler!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        UnregisterClass(g_szTrayHandler, hDllInstance);
        UnregisterClass(g_szTrayIconHandler, hDllInstance);
        return false;
    }

    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hWndMsgHandler, (LPARAM)g_lsMessages);
    return true;
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case LM_GETREVID: {
            size_t uLength;
            StringCchPrintf((LPSTR)lParam, 64, "%s: ", g_szAppName);
			uLength = strlen((LPSTR)lParam);
			GetVersionString(g_version, (LPSTR)lParam + uLength, 64 - uLength, false);
            
            if (SUCCEEDED(StringCchLength((LPSTR)lParam, 64, &uLength)))
                return uLength;

            lParam = NULL;
            return 0;
        }

        case LM_REFRESH:
            return 0;

        case LM_SYSTRAY:
        case LM_SYSTRAYINFOEVENT:
            return TrayManager::ShellMessage(hWnd, uMsg, wParam, lParam);

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates trays.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szName[256];
    LPSTR szTokens[] = { szName };
    LPVOID f = LCOpen(NULL);
    LPSTR name;

    while (LCReadNextConfig(f, "*nTray", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*nTray")+1, szTokens, 1, NULL);
        name = _strdup(szName);
        g_Trays.insert(g_Trays.begin(), std::pair<LPCSTR, Tray*>(name, new Tray(name)));
    }
    LCClose(f);
}
