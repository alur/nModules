/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.cpp
 *  The nModules Project
 *
 *  Implementation of the Tray class. Handles layout of the tray buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Tray.hpp"
#include "TrayIcon.hpp"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTrayHandler;
extern HWND g_hWndTrayNotify;


/// <summary>
/// Constructor
/// </summary>
Tray::Tray(LPCSTR pszName) {
    m_pszName = pszName;

    this->settings = new Settings(m_pszName);

    DrawableSettings* defaults = new DrawableSettings();
    m_pWindow = new DrawableWindow(NULL, g_szTrayHandler, g_hInstance, this->settings, defaults);

    SetWindowLongPtr(m_pWindow->GetWindow(), 0, (LONG_PTR)this);
    m_pWindow->Show();

    LoadSettings();
}


/// <summary>
/// Destructor
/// </summary>
Tray::~Tray() {
    // Remove all icons
    for (vector<TrayIcon*>::const_iterator iter = m_icons.begin(); iter != m_icons.end(); iter++) {
        delete *iter;
    }
    m_icons.clear();

    if (m_pWindow) delete m_pWindow;
    if (this->settings) delete this->settings;
    free((void *)m_pszName);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Tray::LoadSettings(bool /* bIsRefresh */) {
    this->settings->GetOffsetRect("MarginLeft", "MarginTop", "MarginRight", "MarginBottom", &m_rMargin, 2, 2, 5, 2);
    m_iColSpacing = this->settings->GetInt("ColumnSpacing", 2);
    m_iRowSpacing = this->settings->GetInt("RowSpacing", 2);
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
TrayIcon* Tray::AddIcon(LPLSNOTIFYICONDATA pNID) {
    TrayIcon* tIcon = new TrayIcon(m_pWindow->GetWindow(), pNID, this->settings);
    m_icons.push_back(tIcon);
    Relayout();
    tIcon->Show();
    return tIcon;
}


/// <summary>
/// Finds the specified icon.
/// </summary>
vector<TrayIcon*>::const_iterator Tray::FindIcon(TrayIcon* pIcon) {
    for (vector<TrayIcon*>::const_iterator iter = m_icons.begin(); iter != m_icons.end(); iter++) {
        if ((*iter) == pIcon) {
            return iter;
        }
    }
    return m_icons.end();
}


/// <summary>
/// Removes the specified icon from this tray, if it is in it.
/// </summary>
void Tray::RemoveIcon(TrayIcon* pIcon) {
    vector<TrayIcon*>::const_iterator icon = FindIcon(pIcon);
    if (icon != m_icons.end()) {
        delete *icon;
        m_icons.erase(icon);
        Relayout();
    }
}


/// <summary>
/// Repositions/Resizes all icons.
/// </summary>
void Tray::Relayout() {
    int x = m_rMargin.left;
    int y = m_rMargin.top;
    int wrapwidth = this->m_pWindow->GetSettings()->width - m_rMargin.right - 20;

    for (vector<TrayIcon*>::const_iterator iter = m_icons.begin(); iter != m_icons.end(); iter++) {
        (*iter)->Reposition(x, y, 20, 20);
        x += 20 + m_iColSpacing;
        if (x > wrapwidth) {
            x = m_rMargin.left;
            y += 20 + m_iRowSpacing;
        }
    }
}


/// <summary>
/// Handles window events for the tray.
/// </summary>
LRESULT WINAPI Tray::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_MOUSEMOVE:
        if (IsWindow(g_hWndTrayNotify)) {
            DrawableSettings* settings = this->m_pWindow->GetSettings();
            MoveWindow(g_hWndTrayNotify, settings->x, settings->y,
                settings->width, settings->height, FALSE);
        }
    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
