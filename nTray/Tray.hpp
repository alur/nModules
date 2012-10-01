/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.hpp
 *  The nModules Project
 *
 *  Declaration of the Tray class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <map>
#include "TrayIcon.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Settings.hpp"

using std::map;

class Tray {
public:
    explicit Tray(LPCSTR);
    virtual ~Tray();

    void LoadSettings(bool = false);
    TrayIcon* AddIcon(LPLSNOTIFYICONDATA);
    void RemoveIcon(TrayIcon*);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    void Relayout();
    vector<TrayIcon*>::const_iterator FindIcon(TrayIcon* NID);

private:
    // The name of this tray
    LPCSTR name;

    // The tray's window
    DrawableWindow* window;

    // Settings for painting the tray background
    Settings* settings;

    // The tray icons
    vector<TrayIcon*> icons;

    // 
    RECT margin;

    // 
    int rowSpacing;

    // 
    int colSpacing;

    int iconSize;
};
