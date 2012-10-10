/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nDesk.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nDesk module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "nDesk.h"
#include "../nCoreCom/Core.h"
#include "../nShared/MonitorInfo.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"
#include "DesktopPainter.hpp"
#include "ClickHandler.hpp"
#include "WorkArea.h"
#include "Bangs.h"
#include "Settings.h"
#include "ExplorerService.h"


// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// Class pointers
DesktopPainter* g_pDesktopPainter;
MonitorInfo* g_pMonitorInfo;
ClickHandler* g_pClickHandler;
LSModule* g_pLSModule;
ExplorerService* g_pExplorerService;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* path */) {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = instance;
    wc.lpszClassName = "DesktopBackgroundClass";
    wc.hIconSm = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;

    g_pLSModule = new LSModule(parent, "nDesk", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);

    if (!g_pLSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_pLSModule;
        return 1;
    }

    // Initialize
    g_pMonitorInfo = new MonitorInfo();
    g_pClickHandler = new ClickHandler();
    g_pDesktopPainter = NULL; // Initalized on WM_CREATE
    
    if (!g_pLSModule->Initialize(&wc, NULL)) {
        delete g_pLSModule;
        delete g_pMonitorInfo;
        delete g_pClickHandler;
        return 1;
    }

    SetWindowPos(g_pDesktopPainter->GetWindow(), HWND_BOTTOM, g_pMonitorInfo->m_virtualDesktop.rect.left,
        g_pMonitorInfo->m_virtualDesktop.rect.top, g_pMonitorInfo->m_virtualDesktop.width,
        g_pMonitorInfo->m_virtualDesktop.height, SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    ShowWindow(g_pDesktopPainter->GetWindow(), SW_SHOWNOACTIVATE);

    // Load bang commands
    Bangs::_Register();

    // Load settings
    nDesk::Settings::Load();

    // Reset the work area for all monitors
    WorkArea::ResetWorkAreas(g_pMonitorInfo);
    WorkArea::LoadSettings(g_pMonitorInfo);

    g_pExplorerService = new ExplorerService();
    g_pExplorerService->Start();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Reset the work area for all monitors
    WorkArea::ResetWorkAreas(g_pMonitorInfo);

    // Unregister bangs
    Bangs::_Unregister();

    g_pExplorerService->Stop();

    // Delete global classes
    if (g_pDesktopPainter) delete g_pDesktopPainter;
    if (g_pClickHandler) delete g_pClickHandler;
    if (g_pMonitorInfo) delete g_pMonitorInfo;
    if (g_pLSModule) delete g_pLSModule;
    SAFEDELETE(g_pExplorerService);
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
            g_pDesktopPainter = new DesktopPainter(window);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case LM_REFRESH:
        {
            g_pClickHandler->Refresh();
            WorkArea::LoadSettings(g_pMonitorInfo, true);
            nDesk::Settings::Load();
        }
        return 0;
    
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
        {
            g_pClickHandler->HandleClick(message, wParam, lParam);
        }
        return 0;
    
    case WM_PAINT:
    case WM_ERASEBKGND:
        return g_pDesktopPainter->HandleMessage(window, message, wParam, lParam);
    
    case WM_WINDOWPOSCHANGING:
        {
            // Keep the hWnd at the bottom of the window stack
            WINDOWPOS *c = (WINDOWPOS*)lParam;
            c->hwnd = g_pDesktopPainter->GetWindow();
            c->hwndInsertAfter = HWND_BOTTOM;
            c->flags |= SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE;
        }
        return 0;

    case WM_DISPLAYCHANGE:
        {
            g_pMonitorInfo->Update();
            g_pDesktopPainter->Resize();
        }
        break;
    
    case WM_SETTINGCHANGE:
        {
            if (wParam == SPI_SETDESKWALLPAPER) {
                g_pDesktopPainter->UpdateWallpaper();
                return 0;
            }
        }
        break;

    case WM_TIMER:
        switch (wParam) {
        case 1337:
            g_pDesktopPainter->TransitionStep();
        }
        break;
    
    case WM_CLOSE:
        // If someone tries to exit the desktop window, lets make it a windows shutdown.
        // PostMessage(GetLitestepWnd(), LM_RECYCLE, 3, 0);
        return 0;

    case WM_SYSCOMMAND:
        switch (wParam) {
            // For using the standard alt+F4 to shutdown windows
            case SC_CLOSE:
                // PostMessage(GetLitestepWnd(), LM_RECYCLE, 3, 0);
                return 0;
            default:
                break;
        }
        break;

    case WM_CHILDACTIVATE:
    case WM_NCACTIVATE:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    case WM_ACTIVATEAPP:
    case WM_ACTIVATE:
    case WM_PARENTNOTIFY:
        SetWindowPos(g_pDesktopPainter->GetWindow(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        break;
    }
    return DefWindowProc(window, message, wParam, lParam);
}
