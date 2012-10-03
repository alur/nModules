/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.cpp
 *  The nModules Project
 *
 *  Implementation of the Tray class. Handles layout of the tray buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/LSModule.hpp"
#include "Tray.hpp"
#include "TrayIcon.hpp"


extern LSModule* g_LSModule;
extern HWND g_hWndTrayNotify;


/// <summary>
/// Constructor
/// </summary>
Tray::Tray(LPCSTR name) {
    this->name = name;

    this->settings = new Settings(this->name);
    this->layoutSettings = new LayoutSettings();

    DrawableSettings* defaults = new DrawableSettings();
    this->window = new DrawableWindow(NULL, (LPCSTR)g_LSModule->GetWindowClass(1), g_LSModule->GetInstance(), this->settings, defaults, this);
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

    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
    SAFEDELETE(this->layoutSettings);
    free((void *)this->name);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Tray::LoadSettings(bool /* IsRefresh */) {
    LayoutSettings* layoutDefaults = new LayoutSettings();
    this->layoutSettings->Load(this->settings, layoutDefaults);

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
    int x0, y0, xdir, ydir;

    DrawableSettings* drawingSettings = this->window->GetSettings();

    switch (this->layoutSettings->startPosition) {
    default:
    case LayoutSettings::TOPLEFT:
        {
            x0 = this->layoutSettings->padding.left;
            y0 = this->layoutSettings->padding.top;
            xdir = 1;
            ydir = 1;
        }
        break;

    case LayoutSettings::TOPRIGHT:
        {
            x0 = drawingSettings->width - this->layoutSettings->padding.right - this->iconSize;
            y0 = this->layoutSettings->padding.top;
            xdir = -1;
            ydir = 1;
        }
        break;

    case LayoutSettings::BOTTOMLEFT:
        {
            x0 = this->layoutSettings->padding.left;
            y0 = drawingSettings->height - this->layoutSettings->padding.bottom - this->iconSize;
            xdir = 1;
            ydir = -1;
        }
        break;

    case LayoutSettings::BOTTOMRIGHT:
        {
            x0 = drawingSettings->width - this->layoutSettings->padding.right - this->iconSize;
            y0 = drawingSettings->height - this->layoutSettings->padding.bottom - this->iconSize;
            xdir = -1;
            ydir = -1;
        }
        break;
    }

    if (this->layoutSettings->primaryDirection == LayoutSettings::HORIZONTAL) {
        int x = x0, y = y0;
        for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++) {
            (*iter)->Reposition(x, y, this->iconSize, this->iconSize);
            x += xdir*(this->iconSize + this->layoutSettings->columnSpacing);
            if (x < this->layoutSettings->padding.left || x > drawingSettings->width - this->layoutSettings->padding.right - this->iconSize) {
                x = x0;
                y += ydir*(this->iconSize + this->layoutSettings->rowSpacing);
            }
        }
    }
    else {
        int x = x0, y = y0;
        for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++) {
            (*iter)->Reposition(x, y, this->iconSize, this->iconSize);
            y += ydir*(this->iconSize + this->layoutSettings->rowSpacing);
            if (y < this->layoutSettings->padding.top || y > drawingSettings->height - this->layoutSettings->padding.bottom - this->iconSize) {
                y = y0;
                x += xdir*(this->iconSize + this->layoutSettings->columnSpacing);
            }
        }
    }
}


/// <summary>
/// Handles window events for the tray.
/// </summary>
LRESULT WINAPI Tray::HandleMessage(HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_MOUSEMOVE:
        if (IsWindow(g_hWndTrayNotify)) {
            DrawableSettings* settings = this->window->GetSettings();
            MoveWindow(g_hWndTrayNotify, settings->x, settings->y,
                settings->width, settings->height, FALSE);
        }
    default:
        return this->window->HandleMessage(wnd, uMsg, wParam, lParam);
    }
}
