/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.cpp                                                      July, 2012
 *  The nModules Project
 *
 *  Main .cpp file for the nLabel module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "nLabel.h"
#include "../nCoreCom/Core.h"
#include "../nShared/Error.h"
#include "Label.hpp"
#include <map>
#include "../nShared/Factories.h"

using std::map;

// Constants
const VERSION g_minCoreVersion    = MAKE_VERSION(0,2,0,0);
const VERSION g_version           = MAKE_VERSION(0,2,0,0);
LPCSTR g_szAppName                = "nLabel";
LPCSTR g_szMsgHandler             = "LSnLabelMsgHandler";
LPCSTR g_szLabelHandler           = "LSnLabelHandler";
LPCSTR g_szAuthor                 = "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// Handle to the message handler window
HWND g_hWndMsgHandler;

// This instance.
HINSTANCE g_hInstance;

// All the labels we currently have loaded
map<LPCSTR, Label*> g_Labels;


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

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Remove all labels
    for (map<LPCSTR, Label*>::const_iterator iter = g_Labels.begin(); iter != g_Labels.end(); iter++) {
        delete iter->second;
    }
    g_Labels.clear();

    // Deinitalize
    if (g_hWndMsgHandler) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hWndMsgHandler, (LPARAM)g_lsMessages);
        DestroyWindow(g_hWndMsgHandler);
    }

    UnregisterClass(g_szMsgHandler, hDllInstance);
    UnregisterClass(g_szLabelHandler, hDllInstance);

    Factories::Release();
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hDllInstance">The instance to attach this message handler to.</param>
bool CreateLSMsgHandler(HINSTANCE hDllInstance) {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = LSMsgHandlerProc;
    wc.hInstance = hDllInstance;
    wc.lpszClassName = g_szMsgHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nLabel's msg window class!"));
        return false;
    }

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = sizeof(LONG_PTR); // Planning to hold a Label * here.
    wc.lpfnWndProc = LabelHandlerProc;
    wc.hInstance = hDllInstance;
    wc.lpszClassName = g_szLabelHandler;
    wc.hIconSm = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nLabel's label window class!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        return false;
    }

    g_hWndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMsgHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!g_hWndMsgHandler) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to create nLabel's message handler!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        UnregisterClass(g_szLabelHandler, hDllInstance);
        return false;
    }

    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hWndMsgHandler, (LPARAM) g_lsMessages);
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
/// Handles messages for the induvidual label windows.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LabelHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // index 0 of the extra window data holds a pointer to the Label which created it.
    Label * lbl = (Label *)GetWindowLongPtr(hWnd, 0);
    if (lbl) return lbl->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LCOpen(NULL);
    LPSTR name;

    while (LCReadNextConfig(f, "*nLabel", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*nLabel")+1, szTokens, 1, NULL);
        name = _strdup(szLabel);
        g_Labels.insert(g_Labels.begin(), std::pair<LPCSTR, Label*>(name, new Label(name)));
    }
    LCClose(f);
}
