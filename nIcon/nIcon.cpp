/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nDesk.cpp                                                       July, 2012
 *  The nModules Project
 *
 *  Main .cpp file for the nDesk module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "nIcon.h"
#include "../nCoreCom/Core.h"
#include "../nShared/Error.h"

// Constants
const int  g_minCoreVersion = 0x01000000;
LPCSTR g_rcsRevision        = "1.0";
LPCSTR g_szAppName          = "nIcon";
LPCSTR g_szMainHandler      = "DesktopBackgroundClass";
LPCSTR g_szAuthor           = "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND g_hwndMain;


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

    // Initialize
    CreateLSMsgHandler(hDllInstance);

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Deinitalize
    if (g_hwndMain) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM)g_lsMessages);
        DestroyWindow(g_hwndMain);
    }

    UnregisterClass(g_szMainHandler, hDllInstance);
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hInst">The instance to attach this message handler to.</param>
void CreateLSMsgHandler(HINSTANCE hInst) {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = LSMsgHandlerProc;
    wc.hInstance = hInst;
    wc.lpszClassName = g_szMainHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Error registering Desktop window class"), TEXT("Debug"), MB_OK);
        return;
    }

    g_hwndMain = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMainHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hInst, NULL);

    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM) g_lsMessages);

    SetWindowLongPtr(g_hwndMain, GWLP_USERDATA, MAGIC_DWORD);
    SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
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
            StringCchPrintf((char*)lParam, 64, "%s: %s", g_szAppName, g_rcsRevision);
            
            if (SUCCEEDED(StringCchLength((char*)lParam, 64, &uLength)))
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
