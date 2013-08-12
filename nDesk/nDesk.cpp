/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nDesk.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nDesk module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nDesk.h"
#include "../nCoreCom/Core.h"
#include "../nShared/MonitorInfo.hpp"
#include "../nShared/LSModule.hpp"
#include "DesktopPainter.hpp"
#include "ClickHandler.hpp"
#include "WorkArea.h"
#include "Bangs.h"
#include "Settings.h"
#include <dwmapi.h>
#include "../nShared/Color.h"
#include "Version.h"
#include <ShObjIdl.h>


// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// Class pointers
DesktopPainter *g_pDesktopPainter;
MonitorInfo *g_pMonitorInfo;
ClickHandler *g_pClickHandler;

// The LSModule class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */) {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = instance;
    wc.lpszClassName = _T("DesktopBackgroundClass");
    wc.hIconSm = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;
    
    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
        return 1;
    }

    // Initialize
    g_pMonitorInfo = new MonitorInfo();
    g_pClickHandler = new ClickHandler();
    g_pDesktopPainter = nullptr; // Initialized on WM_CREATE
    
    if (!gLSModule.Initialize(parent, instance, &wc, nullptr)) {
        delete g_pMonitorInfo;
        delete g_pClickHandler;
        return 1;
    }

    SetParent(g_pDesktopPainter->GetWindow(), GetDesktopWindow());
    SetWindowLongPtr(g_pDesktopPainter->GetWindow(), GWL_STYLE, GetWindowLongPtr(g_pDesktopPainter->GetWindow(), GWL_STYLE) | WS_CHILD | WS_CLIPCHILDREN);
    SetWindowPos(g_pDesktopPainter->GetWindow(), HWND_BOTTOM, g_pMonitorInfo->m_virtualDesktop.rect.left,
        g_pMonitorInfo->m_virtualDesktop.rect.top, g_pMonitorInfo->m_virtualDesktop.width,
        g_pMonitorInfo->m_virtualDesktop.height, SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    ShowWindow(g_pDesktopPainter->GetWindow(), SW_SHOWNOACTIVATE);
    BOOL excludeFromPeek = TRUE;
    DwmSetWindowAttribute(g_pDesktopPainter->GetWindow(), DWMWA_EXCLUDED_FROM_PEEK, &excludeFromPeek, sizeof(BOOL));

    // Load bang commands
    Bangs::_Register();

    // Load settings
    nDesk::Settings::Load();

    // Reset the work area for all monitors
    WorkArea::ResetWorkAreas(g_pMonitorInfo);
    WorkArea::LoadSettings(g_pMonitorInfo);

    /*const CLSID CLSID_Test = {0xff609cc7, 0xd34d, 0x4049, {0xa1, 0xaa, 0x22, 0x93, 0x51, 0x7f, 0xfc, 0xc6}};
    IExecuteCommand *iec;
    IObjectWithSelection *iows;
    IShellItem *ishi;
    IShellItemArray *ishia;

    HRESULT hr = CoCreateInstance(CLSID_Test, nullptr, CLSCTX_INPROC_SERVER, IID_IExecuteCommand, (LPVOID*)&iec);

    if (SUCCEEDED(hr)) {
        hr = iec->QueryInterface(IID_IObjectWithSelection, (LPVOID*)&iows);

        hr = SHCreateItemFromParsingName(L"C:\\Users\\Erik\\Dropbox\\Wallpapers\\24545.jpg", nullptr, IID_IShellItem, (LPVOID*)&ishi);

        hr = SHCreateShellItemArrayFromShellItem(ishi, IID_IShellItemArray, (LPVOID*)&ishia);

        hr = iec->SetDirectory(L"C:\\Users\\Erik\\Dropbox\\Wallpapers");
        hr = iows->SetSelection(ishia);

        hr = iec->Execute();
        
        iec->Release();
    }*/

    // Used to generate the table for: http://portfolio.alurcard2.net/#!/wiki/Named%20Colors
    
    ////TRACE("[tr][th]Name[/th][th]Color[/th][th]#ARGB[/th][th]Alpha[/th][th]RGB[/th][th]HSL[/th][th]HSV[/th][/tr]");
    //TRACE("|| Name || `_____ Color of the color _____` || #ARGB || Alpha || RGB || HSL || HSV ||");
    //Color::KnownColor* knownColors = Color::GetKnownColors();
    //for (int i = 0; knownColors[i].name != NULL; i++) {
    //    AHSL ahsl = Color::ARGBToAHSL(knownColors[i].color);
    //    AHSV ahsv = Color::ARGBToAHSV(knownColors[i].color);

    //    /*TRACE("[tr][td]%s[/td][td][coloredblock=rgba(%u,%u,%u,%f) width=100%%][/td][td]#%08X[/td][td]%u[/td][td]%u, %u, %u[/td][td]%u, %u, %u[/td][td]%u, %u, %u[/td][/tr]",
    //        knownColors[i].name, // Name
    //        knownColors[i].color >> 16 & 0xFF, knownColors[i].color >> 8 & 0xFF, knownColors[i].color & 0xFF, (knownColors[i].color >> 24) / 255.0f,  // Block color
    //        knownColors[i].color, knownColors[i].color >> 24, // #ARGB, Alpha
    //        knownColors[i].color >> 16 & 0xFF, knownColors[i].color >> 8 & 0xFF, knownColors[i].color & 0xFF, // RGB
    //        ahsl.hue, ahsl.saturation, ahsl.lightness, ahsv.hue, ahsv.saturation, ahsv.value // HSL, HSV
    //        );*/

    //    TRACE("|| `%s` || http://alurcard2.net/color.svg?color=%u,%u,%u,%.2f || #%08X || %u || %u, %u, %u || %u, %u, %u || %u, %u, %u ||",
    //        knownColors[i].name, // Name
    //        knownColors[i].color >> 16 & 0xFF, knownColors[i].color >> 8 & 0xFF, knownColors[i].color & 0xFF, (knownColors[i].color >> 24) / 255.0f,  // Block color
    //        knownColors[i].color, knownColors[i].color >> 24, // #ARGB, Alpha
    //        knownColors[i].color >> 16 & 0xFF, knownColors[i].color >> 8 & 0xFF, knownColors[i].color & 0xFF, // RGB
    //        ahsl.hue, (UINT)ahsl.saturation, (UINT)ahsl.lightness, ahsv.hue, (UINT)ahsv.saturation, (UINT)ahsv.value // HSL, HSV
    //        );
    //}

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

    // Delete global classes
    if (g_pDesktopPainter) delete g_pDesktopPainter;
    if (g_pClickHandler) delete g_pClickHandler;
    if (g_pMonitorInfo) delete g_pMonitorInfo;

    //
    gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message)
    {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
            g_pDesktopPainter = new DesktopPainter(window);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case LM_REFRESH:
        {
            g_pClickHandler->Refresh();
            WorkArea::LoadSettings(g_pMonitorInfo, true);
            nDesk::Settings::Load();
        }
        return 0;
    
    case WM_PAINT:
    case WM_ERASEBKGND:
        return g_pDesktopPainter->HandleMessage(window, message, wParam, lParam);
    
    case WM_WINDOWPOSCHANGING:
        {
            // Keep the hWnd at the bottom of the window stack
            LPWINDOWPOS c = LPWINDOWPOS(lParam);
            c->hwnd = window;
            c->hwndInsertAfter = HWND_BOTTOM;
            c->flags &= ~SWP_HIDEWINDOW;
            c->flags |= SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_SHOWWINDOW;
            c->x = g_pMonitorInfo->m_virtualDesktop.rect.left;
            c->y = g_pMonitorInfo->m_virtualDesktop.rect.top;
            c->cx = g_pMonitorInfo->m_virtualDesktop.width;
            c->cy = g_pMonitorInfo->m_virtualDesktop.height;
        }
        return 0;

    case WM_DISPLAYCHANGE:
        {
            g_pMonitorInfo->Update();
            g_pDesktopPainter->Resize();
            nDesk::Settings::OnResolutionChange();
            WorkArea::LoadSettings(g_pMonitorInfo);
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
        SetWindowPos(window, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        break;
    }

    if (g_pDesktopPainter) {
        return g_pDesktopPainter->HandleMessage(window, message, wParam, lParam);
    }

    return DefWindowProc(window, message, wParam, lParam);
}
