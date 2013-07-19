/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nCore module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "nCore.h"
#include "../nShared/ErrorHandler.h"
#include "../nShared/MonitorInfo.hpp"
#include "../nShared/Versioning.h"
#include "TextFunctions.h"
#include "ParsedText.hpp"
#include "Version.h"


// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND g_hwndMsgHandler;

// Class pointers
MonitorInfo *g_pMonitorInfo;

// Constants
LPCSTR g_szMsgHandler = "LSnCore";

// When the [time] text function should send out change notifications.
UINT_PTR timeTimer;


/// <summary>
/// Gets the current core version.
/// </summary>
EXPORT_CDECL(VERSION) GetCoreVersion() {
    return MakeVersion(MODULE_VERSION);
}


/// <summary>
/// Called by the core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* parent */, HINSTANCE instance, LPCSTR /* path */) {
    // Initialize
    if (!CreateMainWindow(instance))
        return 1;

    g_pMonitorInfo = new MonitorInfo();
    TextFunctions::_Register();
    timeTimer = SetTimer(g_hwndMsgHandler, 1, 1000, nullptr);

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
    // Deinitalize
    if (g_hwndMsgHandler) {
        KillTimer(g_hwndMsgHandler, timeTimer);
        SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hwndMsgHandler, (LPARAM)g_lsMessages);
        DestroyWindow(g_hwndMsgHandler);
    }

    TextFunctions::_Unregister();

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
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Failed to register nCore's window class!"));
        return false;
    }

    g_hwndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMsgHandler, "",
        WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!g_hwndMsgHandler) {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Failed to create nCore's message handler!"));
        UnregisterClass(g_szMsgHandler, hDllInstance);
        return false;
    }

    // Let other modules get the instance using FindWindow + GetWindowLongPtr()
    SetWindowLongPtr(g_hwndMsgHandler, GWLP_USERDATA, (LONG_PTR)hDllInstance);
    SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hwndMsgHandler, (LPARAM) g_lsMessages);

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
            StringCchPrintf((LPSTR)lParam, 64, "%s: ", MODULE_NAME);
			uLength = strlen((LPSTR)lParam);
            GetVersionString(MakeVersion(MODULE_VERSION), (LPSTR)lParam + uLength, 64 - uLength, false);
            
            if (SUCCEEDED(StringCchLength((LPSTR)lParam, 64, &uLength)))
                return uLength;

            lParam = NULL;
            return 0;
        }
        case LM_REFRESH: {
            return 0;
        }
        case WM_TIMER:
            {
                if (wParam == timeTimer) {
                    DynamicTextChangeNotification(L"Time", 0);
                    DynamicTextChangeNotification(L"Time", 1);
                    DynamicTextChangeNotification(L"WindowTitle", 1);
                }
            }
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
