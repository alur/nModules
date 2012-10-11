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
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/Tooltip.hpp"

using std::map;

class Tray : public Drawable {
public:
    explicit Tray(LPCSTR);
    virtual ~Tray();

    TrayIcon* AddIcon(LPLSNOTIFYICONDATA);
    vector<TrayIcon*>::const_iterator FindIcon(TrayIcon* NID);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void InitCompleted();
    void LoadSettings(bool = false);
    void ShowTip(int x, int y, LPCWSTR text);
    void Relayout();
    void RemoveIcon(TrayIcon*);
    void HideTip();

private:
    // The tray icons.
    vector<TrayIcon*> icons;

    // The size of the icons.
    int iconSize;

    // Defines how the tray icons should be layed out.
    LayoutSettings* layoutSettings;

    // The tooltip to show.
    Tooltip* tooltip;
};
