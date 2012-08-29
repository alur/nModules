/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.cpp                                                       July, 2012
 *  The nModules Project
 *
 *  Main .cpp file for the nCore module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "nCore.h"
#include "../nShared/Error.h"
#include "../nShared/MonitorInfo.hpp"
#include "../nShared/Versioning.h"

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND g_hwndMsgHandler;

// Class pointers
MonitorInfo* g_pMonitorInfo;

// Constants
const VERSION g_version     = MAKE_VERSION(0,2,0,0);
LPCSTR g_szAppName          = "nCore";
LPCSTR g_szMsgHandler       = "LSnCore";
LPCSTR g_szAuthor           = "Alurcard2";


/// <summary>
/// Gets the current core version.
/// </summary>
EXPORT_CDECL(VERSION) GetCoreVersion() {
    return g_version;
}


/// <summary>
/// The main entry point for this DLL.
/// </summary>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls((HINSTANCE)hModule);
    return TRUE;
}


/// <summary>
/// Called by the core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE hDllInstance, LPCSTR /* szPath */) {
    // Initialize
    if (!CreateMainWindow(hDllInstance))
        return 1;

    g_pMonitorInfo = new MonitorInfo();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Deinitalize
    if (g_hwndMsgHandler) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hwndMsgHandler, (LPARAM)g_lsMessages);
        DestroyWindow(g_hwndMsgHandler);
    }

    UnregisterClass(g_szMsgHandler, hDllInstance);
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hDllInstance">The instance to attach this message handler to.</param>
bool CreateMainWindow(HINSTANCE hDllInstance) {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MainProc;
    wc.hInstance = hDllInstance;
    wc.lpszClassName = g_szMsgHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nCore's window class!"));
        return false;
    }

    g_hwndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMsgHandler, "",
        WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!g_hwndMsgHandler) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to create nCore's message handler!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        return false;
    }

    // Let other modules get the instance using FindWindow + GetWindowLongPtr()
    SetWindowLongPtr(g_hwndMsgHandler, GWLP_USERDATA, (LONG_PTR)hDllInstance);
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hwndMsgHandler, (LPARAM) g_lsMessages);

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
        case LM_REFRESH: {
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
