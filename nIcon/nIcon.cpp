/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nIcon module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "nIcon.h"
#include "../nCoreCom/Core.h"
#include "../nShared/Error.h"
#include "IconGroup.hpp"
#include "../nShared/Factories.h"

// Constants
const VERSION g_minCoreVersion    = MAKE_VERSION(0,2,0,0);
const VERSION g_version           = MAKE_VERSION(0,2,0,0);
LPCSTR g_szAppName                = "nIcon";
LPCSTR g_szMainHandler            = "nIconMsgHandler";
LPCSTR g_szGroupHandler           = "nIconGroupHandler";
LPCSTR g_szIconHandler           = "nIconIconHandler";
LPCSTR g_szAuthor                 = "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND g_hwndMain;
HINSTANCE g_hInstance;

IconGroup* pGroup;

/// <summary>
/// The main entry point for this DLL.
/// </summary>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls((HINSTANCE)hModule);
    return TRUE;
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE hDllInstance, LPCSTR /* szPath */) {
    // Initalize communication with the core
    switch (nCore::Init(g_minCoreVersion)) {
    case S_OK:
        break;
    default:
        ErrorMessage(E_LVL_ERROR, "There was a problem connecting to nCore!");
        return 1;
    }

    g_hInstance = hDllInstance;

    // Initialize
    if (!CreateMessageHandlers(hDllInstance)) {
        return 1;
    }

    pGroup = new IconGroup("DesktopIcons");

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    if (pGroup)
        delete pGroup;

    // Deinitalize
    if (g_hwndMain) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM)g_lsMessages);
        DestroyWindow(g_hwndMain);
    }

    UnregisterClass(g_szMainHandler, hDllInstance);
    UnregisterClass(g_szGroupHandler, hDllInstance);
    UnregisterClass(g_szIconHandler, hDllInstance);

    // Let go of any factories we've allocated
    Factories::Release();
}


/// <summary>
/// Creates message handlers
/// </summary>
/// <param name="hInst">The instance to attach this message handler to.</param>
bool CreateMessageHandlers(HINSTANCE hInst) {
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = LSMsgHandlerProc;
    wc.hInstance = hInst;
    wc.lpszClassName = g_szMainHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nIcon's msg window class!"));
        return false;
    }

    wc.cbWndExtra = sizeof(LONG_PTR); // Planning to hold a IconGroup * here.
    wc.lpfnWndProc = GroupHandlerProc;
    wc.lpszClassName = g_szGroupHandler;
    wc.style = CS_NOCLOSE | CS_DBLCLKS;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nIcon's group class!"));
        UnregisterClass(g_szMainHandler, hInst);
        return false;
    }

    wc.cbWndExtra = sizeof(LONG_PTR); // Planning to hold a Icon * here.
    wc.lpfnWndProc = IconHandlerProc;
    wc.lpszClassName = g_szIconHandler;
    wc.style = CS_NOCLOSE | CS_DBLCLKS;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nIcon's icon class!"));
        UnregisterClass(g_szMainHandler, hInst);
        UnregisterClass(g_szGroupHandler, hInst);
        return false;
    }

    g_hwndMain = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMainHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hInst, NULL);

    if (!g_hwndMain) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to create nIcon's message handler window!"));
        UnregisterClass(g_szMainHandler, hInst);
        UnregisterClass(g_szGroupHandler, hInst);
        UnregisterClass(g_szIconHandler, hInst);
        return false;
    }

    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM)g_lsMessages);

    SetWindowLongPtr(g_hwndMain, GWLP_USERDATA, MAGIC_DWORD);
    SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);

    return true;
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMsgHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
        case LM_REFRESH: {
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Handles messages for the icongroups.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI GroupHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // index 0 of the extra window data holds a pointer to the TaskButton which created it.
    IconGroup * pGroup = (IconGroup *)GetWindowLongPtr(hWnd, 0);
    if (pGroup) return pGroup->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Handles messages for the icongroups.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI IconHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // index 0 of the extra window data holds a pointer to the TaskButton which created it.
    Icon * pIcon = (Icon *)GetWindowLongPtr(hWnd, 0);
    if (pIcon) return pIcon->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
