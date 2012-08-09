/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTask.cpp                                                       July, 2012
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "nTask.h"
#include "Taskbar.hpp"
#include "../nShared/nShared.h"
#include "../nCoreCom/Core.h"
#include <map>
#include "WindowManager.h"
#include "../nShared/Factories.h"

using std::map;

const VERSION g_minCoreVersion  = 0x01000000;
LPCSTR g_rcsRevision            = "1.0";
LPCSTR g_szAppName              = "nTask";
LPCSTR g_szMsgHandler           = "LSnTaskMsgHandler";
LPCSTR g_szTaskbarHandler       = "LSnTaskbarHandler";
LPCSTR g_szTaskButtonHandler    = "LSnTaskButtonHandler";
LPCSTR g_szAuthor               = "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, LM_WINDOWCREATED, LM_WINDOWACTIVATED,
    LM_WINDOWDESTROYED, LM_LANGUAGE, LM_REDRAW, LM_GETMINRECT, LM_WINDOWREPLACED,
    LM_WINDOWREPLACING, LM_MONITORCHANGED, 0 };

// Handle to the message handler window
HWND g_hWndMsgHandler;

// This instance.
HINSTANCE g_hInstance;

// All the labels we currently have loaded
map<LPCSTR, Taskbar*> g_Taskbars;


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

    // Start the window manager
    WindowManager::Start();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Stop the window manager
    WindowManager::Stop();

    // Remove all taskbars
    for (map<LPCSTR, Taskbar*>::const_iterator iter = g_Taskbars.begin(); iter != g_Taskbars.end(); iter++) {
        delete iter->second;
    }
    g_Taskbars.clear();

    // Deinitalize
    if (g_hWndMsgHandler) {
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hWndMsgHandler, (LPARAM)g_lsMessages);
        DestroyWindow(g_hWndMsgHandler);
    }

    UnregisterClass(g_szMsgHandler, hDllInstance);
    UnregisterClass(g_szTaskbarHandler, hDllInstance);
    UnregisterClass(g_szTaskButtonHandler, hDllInstance);

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
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTaskbars's msg window class!"));
        return false;
    }
    
    // Register the taskbar window class
    wc.cbWndExtra = sizeof(LONG_PTR); // Planning to hold a Taskbar * here.
    wc.lpfnWndProc = TaskbarHandlerProc;
    wc.lpszClassName = g_szTaskbarHandler;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTaskbars's taskbar window class!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        return false;
    }

    // Register the taskbutton window class
    wc.lpfnWndProc = TaskButtonHandlerProc;
    wc.lpszClassName = g_szTaskButtonHandler;

    if (!RegisterClassEx(&wc)) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to register nTaskbars's button window class!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        UnregisterClass(g_szTaskbarHandler, hDllInstance);
        return false;
    }

    // Create the LiteStep message handler window
    g_hWndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMsgHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!g_hWndMsgHandler) {
        ErrorMessage(E_LVL_ERROR, TEXT("Failed to create nTaskbars's message handler!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        UnregisterClass(g_szTaskbarHandler, hDllInstance);
        UnregisterClass(g_szTaskButtonHandler, hDllInstance);
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
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case LM_GETREVID: {
            size_t uLength;
            StringCchPrintf((char*)lParam, 64, "%s: %s", g_szAppName, g_rcsRevision);
            
            if (SUCCEEDED(StringCchLength((char*)lParam, 64, &uLength)))
                return uLength;

            lParam = NULL;
            return 0;
        }

        case LM_REFRESH:
            return 0;

        case LM_GETMINRECT:
        case LM_LANGUAGE:
        case LM_REDRAW:
        case LM_WINDOWACTIVATED:
        case LM_WINDOWCREATED:
        case LM_WINDOWDESTROYED:
        case LM_WINDOWREPLACED:
        case LM_WINDOWREPLACING:
        case LM_MONITORCHANGED:
        case WM_DISPLAYCHANGE:
        case WM_TIMER:
            return WindowManager::ShellMessage(hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Handles messages for the induvidual taskbars.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI TaskbarHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // index 0 of the extra window data holds a pointer to the TaskBar which created it.
    Taskbar * pTask = (Taskbar *)GetWindowLongPtr(hWnd, 0);
    if (pTask) return pTask->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Handles messages for the induvidual task buttons.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI TaskButtonHandlerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // index 0 of the extra window data holds a pointer to the TaskButton which created it.
    TaskButton * pTask = (TaskButton *)GetWindowLongPtr(hWnd, 0);
    if (pTask) return pTask->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LCOpen(NULL);
    LPSTR name;

    while (LCReadNextConfig(f, "*nTaskbar", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*nTaskbar")+1, szTokens, 1, NULL);
        name = _strdup(szLabel);
        g_Taskbars.insert(g_Taskbars.begin(), std::pair<LPCSTR, Taskbar*>(name, new Taskbar(name)));
    }
    LCClose(f);
}
