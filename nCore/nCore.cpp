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
#include "../nCoreCom/Core.h"
#include "../nShared/ErrorHandler.h"
#include "../nShared/MonitorInfo.hpp"
#include "../Utilities/Versioning.h"
#include "../nShared/WindowBangs.h"
#include "../nShared/StateBangs.h"
#include "../nShared/BrushBangs.h"
#include "TextFunctions.h"
#include "ParsedText.hpp"
#include "Version.h"
#include "Scripting.h"


// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND ghWndMsgHandler;

// Class pointers
MonitorInfo *g_pMonitorInfo;

// Constants
LPCTSTR gMsgHandler = _T("LSnCore");

// When the [time] text function should send out change notifications.
UINT_PTR timeTimer;

// 
EXPORT_CDECL(Window*) FindRegisteredWindow(LPCTSTR prefix);


/// <summary>
/// Gets the current core version.
/// </summary>
EXPORT_CDECL(VERSION) GetCoreVersion()
{
    return MakeVersion(MODULE_VERSION);
}


/// <summary>
/// Called by the core when this module is loaded.
/// </summary>
int initModuleW(HWND /* parent */, HINSTANCE instance, LPCWSTR /* path */)
{
    // Initialize
    if (!CreateMainWindow(instance))
    {
        return 1;
    }

    g_pMonitorInfo = new MonitorInfo();
    TextFunctions::_Register();
    timeTimer = SetTimer(ghWndMsgHandler, 1, 1000, nullptr);

    // We need to be connected to the core for some of the functions in nShared to work... xD
    nCore::Connect(MakeVersion(MODULE_VERSION));

    // Register window bangs
    WindowBangs::Register(_T("n"), FindRegisteredWindow);
    StateBangs::Register(_T("n"), FindRegisteredWindow);
    BrushBangs::Register(_T("n"), FindRegisteredWindow);

    // Start the scripting manager
    Scripting::Initialize();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance)
{
    Scripting::Shutdown();

    WindowBangs::UnRegister(_T("n"));
    StateBangs::UnRegister(_T("n"));
    BrushBangs::UnRegister(_T("n"));

    // Deinitalize
    if (ghWndMsgHandler)
    {
        KillTimer(ghWndMsgHandler, timeTimer);
        SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)ghWndMsgHandler, (LPARAM)gLSMessages);
        DestroyWindow(ghWndMsgHandler);
    }

    TextFunctions::_Unregister();

    UnregisterClass(gMsgHandler, hDllInstance);
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hDllInstance">The instance to attach this message handler to.</param>
bool CreateMainWindow(HINSTANCE hDllInstance)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MainProc;
    wc.hInstance = hDllInstance;
    wc.lpszClassName = gMsgHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassEx(&wc))
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Failed to register nCore's window class!"));
        return false;
    }

    ghWndMsgHandler = CreateWindowEx(WS_EX_TOOLWINDOW, gMsgHandler, _T(""),
        WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 0, 0, NULL, NULL, hDllInstance, NULL);

    if (!ghWndMsgHandler)
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Failed to create nCore's message handler!"));
        UnregisterClass(gMsgHandler, hDllInstance);
        return false;
    }

    // Let other modules get the instance using FindWindow + GetWindowLongPtr()
    SetWindowLongPtr(ghWndMsgHandler, GWLP_USERDATA, (LONG_PTR)hDllInstance);
    SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)ghWndMsgHandler, (LPARAM)gLSMessages);

    return true;
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case LM_GETREVID:
        {
            size_t length;
            StringCchPrintf((LPTSTR)lParam, 64, TEXT("%s: "), _T(MODULE_NAME));
			length = lstrlen((LPTSTR)lParam);
            GetVersionString(MakeVersion(MODULE_VERSION), (LPTSTR)lParam + length, 64 - length, false);
            
            if (SUCCEEDED(StringCchLength((LPTSTR)lParam, 64, &length)))
            {
                return length;
            }

            lParam = NULL;
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == timeTimer)
            {
                DynamicTextChangeNotification(L"Time", 0);
                DynamicTextChangeNotification(L"Time", 1);
                DynamicTextChangeNotification(L"WindowTitle", 1);
            }
        }
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
