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
Tray::Tray(LPCSTR name) {
    this->name = name;

    this->settings = new Settings(this->name);

    DrawableSettings* defaults = new DrawableSettings();
    this->window = new DrawableWindow(NULL, g_szTrayHandler, g_hInstance, this->settings, defaults);

    SetWindowLongPtr(this->window->GetWindow(), 0, (LONG_PTR)this);
    this->window->Show();

    LoadSettings();
}


/// <summary>
/// Destructor
/// </summary>
Tray::~Tray() {
    // Remove all icons
    for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++) {
        delete *iter;
    }
    this->icons.clear();

    if (this->window) delete this->window;
    if (this->settings) delete this->settings;
    free((void *)this->name);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Tray::LoadSettings(bool /* IsRefresh */) {
    this->settings->GetOffsetRect("MarginLeft", "MarginTop", "MarginRight", "MarginBottom", &this->margin, 2, 2, 5, 2);
    this->colSpacing = this->settings->GetInt("ColumnSpacing", 2);
    this->rowSpacing = this->settings->GetInt("RowSpacing", 2);

    Settings* iconSettings = this->settings->CreateChild("Icon");
    this->iconSize = iconSettings->GetInt("Size", 16);
    delete iconSettings;
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
TrayIcon* Tray::AddIcon(LPLSNOTIFYICONDATA NID) {
    TrayIcon* icon = new TrayIcon(this->window->GetWindow(), NID, this->settings);
    this->icons.push_back(icon);
    Relayout();
    icon->Show();
    return icon;
}


/// <summary>
/// Finds the specified icon.
/// </summary>
vector<TrayIcon*>::const_iterator Tray::FindIcon(TrayIcon* pIcon) {
    for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++) {
        if ((*iter) == pIcon) {
            return iter;
        }
    }
    return this->icons.end();
}


/// <summary>
/// Removes the specified icon from this tray, if it is in it.
/// </summary>
void Tray::RemoveIcon(TrayIcon* pIcon) {
    vector<TrayIcon*>::const_iterator icon = FindIcon(pIcon);
    if (icon != this->icons.end()) {
        delete *icon;
        this->icons.erase(icon);
        Relayout();
    }
}


/// <summary>
/// Repositions/Resizes all icons.
/// </summary>
void Tray::Relayout() {
    int x = this->margin.left;
    int y = this->margin.top;
    int wrapwidth = this->window->GetSettings()->width - this->margin.right - this->iconSize;

    for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++) {
        (*iter)->Reposition(x, y, this->iconSize, this->iconSize);
        x += this->iconSize + this->colSpacing;
        if (x > wrapwidth) {
            x = this->margin.left;
            y += this->iconSize + this->colSpacing;
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
            DrawableSettings* settings = this->window->GetSettings();
            MoveWindow(g_hWndTrayNotify, settings->x, settings->y,
                settings->width, settings->height, FALSE);
        }
    default:
        return this->window->HandleMessage(uMsg, wParam, lParam);
    }
}
