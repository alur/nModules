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
TrayIcon::TrayIcon(HWND parent, LPLSNOTIFYICONDATA pNID, Settings* parentSettings) {
    //
    LoadSettings();

    //
    m_pNotifyData = pNID;

    // Create the drawable window
    this->settings = parentSettings->CreateChild("Icon");
    m_pWindow = new DrawableWindow(parent, g_szTrayIconHandler, g_hInstance, this->settings, new DrawableSettings());
    SetWindowLongPtr(m_pWindow->GetWindow(), 0, (LONG_PTR)this);

    //
    UpdateIcon();
}


/// <summary>
/// Destructor
/// </summary>
TrayIcon::~TrayIcon() {
    if (m_pWindow) delete m_pWindow;
    if (this->settings) delete this->settings;
}


/// <summary>
/// Loads RC settings.
/// </summary>
void TrayIcon::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
}


/// <summary>
/// Shows the trayicon.
/// </summary>
void TrayIcon::Show() {
    m_pWindow->Show();
}


/// <summary>
/// Retrives the icon's HWND.
/// </summary>
HWND TrayIcon::GetHWND() {
    return m_pWindow->GetWindow();
}


/// <summary>
/// Updates the icon.
/// </summary>
void TrayIcon::UpdateIcon() {
    m_pWindow->PurgeOverlays();
    if ((m_pNotifyData->uFlags & NIF_ICON) == NIF_ICON) {
        D2D1_RECT_F f;
        f.bottom = 18; f.top = 2; f.left = 2; f.right = 18;
        m_pWindow->AddOverlay(f, m_pNotifyData->hIcon);
    }
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
    DrawableSettings* drawableSettings = this->m_pWindow->GetSettings();
    drawableSettings->x = x;
    drawableSettings->y = y;
    drawableSettings->width = width;
    drawableSettings->height = height;
    m_pWindow->UpdatePosition();
}


/// <summary>
/// Sends a message to the owner of the icon.
/// </summary>
void TrayIcon::SendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (m_pNotifyData->uVersion >= 4) {
        RECT r;
        GetWindowRect(m_pWindow->GetWindow(), &r);
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)MAKEWPARAM(r.left, r.top), (LPARAM)MAKELPARAM(uMsg, m_pNotifyData->uID));
    }
    else {
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)m_pNotifyData->uID, (LPARAM)uMsg);
    }
}


/// <summary>
/// Handles window messages for the icon's window.
/// </summary>
LRESULT WINAPI TrayIcon::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg > WM_MOUSEFIRST && uMsg < WM_MOUSELAST) {
        SendCallback(uMsg, wParam, lParam);
        if (uMsg == WM_RBUTTONUP) {
            SendCallback(WM_CONTEXTMENU, wParam, lParam);
        }
        else if (uMsg == WM_LBUTTONUP) {
            SendCallback(NIN_SELECT, wParam, lParam);
        }
    }
    else {
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
    return 0;
}
