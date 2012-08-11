/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayIcon.cpp                                                    July, 2012
 *  The nModules Project
 *
 *  Implementation of the TrayIcon class. Represents a taskbar button.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "TrayIcon.hpp"
#include "Windowsx.h"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTrayIconHandler;


/// <summary>
/// Constructor
/// </summary>
TrayIcon::TrayIcon(HWND parent, LPCSTR prefix, LPLSNOTIFYICONDATA pNID) {
    //
    LoadSettings();

    m_pNotifyData = pNID;

    // Create the drawable window
    m_pPaintSettings = new PaintSettings("TrayIcon");
    m_pWindow = new DrawableWindow(parent, g_szTrayIconHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);

    if ((m_pNotifyData->uFlags & NIF_ICON) == NIF_ICON) {
        D2D1_RECT_F f;
        f.bottom = 18; f.top = 2; f.left = 2; f.right = 18;
        m_pWindow->AddOverlay(f, m_pNotifyData->hIcon);
    }
}


/// <summary>
/// Destructor
/// </summary>
TrayIcon::~TrayIcon() {
    if (m_pWindow) delete m_pWindow;
    if (m_pPaintSettings) delete m_pPaintSettings;
}


void TrayIcon::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
}


void TrayIcon::Show() {
    m_pWindow->Show();
}


HWND TrayIcon::GetHWND() {
    return m_pWindow->getWindow();
}


void TrayIcon::UpdateIcon() {
    m_pWindow->PurgeOverlays();
    if ((m_pNotifyData->uFlags & NIF_ICON) == NIF_ICON) {
        D2D1_RECT_F f;
        f.bottom = 18; f.top = 2; f.left = 2; f.right = 18;
        m_pWindow->AddOverlay(f, m_pNotifyData->hIcon);
    }
}


void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
    m_pPaintSettings->position.left = x;
    m_pPaintSettings->position.top = y;
    m_pPaintSettings->position.right = x + width;
    m_pPaintSettings->position.bottom = y + height;
    m_pWindow->UpdatePosition();
}


void TrayIcon::SendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (m_pNotifyData->uVersion == 0)
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)m_pNotifyData->uID, (LPARAM)uMsg);
    else if (m_pNotifyData->uVersion >= 4) {
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)MAKEWPARAM(1720, 1150), (LPARAM)MAKELPARAM(uMsg, m_pNotifyData->uID));
    }
}


LRESULT WINAPI TrayIcon::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    //case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
        SendCallback(uMsg, wParam, lParam);
        return 0;
    case WM_RBUTTONUP:
        SendCallback(uMsg, wParam, lParam);
        SendCallback(WM_CONTEXTMENU, wParam, lParam);
        return 0;
    case WM_LBUTTONUP:
        //SendCallback(WM_LBUTTONDOWN, wParam, lParam);
        //SendCallback(uMsg, wParam, lParam);
        SendCallback(NIN_SELECT, wParam, lParam);
        return 0;
    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
