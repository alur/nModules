/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.hpp
 *  The nModules Project
 *
 *  A systen tray, contains the tray icons.
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

    // Adds an icon to the tray.
    TrayIcon* AddIcon(LiteStep::LPLSNOTIFYICONDATA);

    // Removes the specified icon from this tray, if it exists.
    void RemoveIcon(TrayIcon*);

    // Handles window messages for the tray.
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

    // Should be called when the initalization phase is done -- repaint the tray.
    void InitCompleted();

    // Shows the trays tooltip.
    void ShowTip(LPCWSTR text, LPRECT position);

    // 
    void HideTip();

private:
    // Loads .rc settings for this tray.
    void LoadSettings(bool isRefresh = false);

    // Positions all tray icons properly.
    void Relayout();

    // Finds the specified icon in the tray.
    vector<TrayIcon*>::const_iterator FindIcon(TrayIcon* NID);

    // The tray icons.
    vector<TrayIcon*> icons;

    // The size of the icons.
    int iconSize;

    // Defines how the tray icons should be layed out.
    LayoutSettings* layoutSettings;

    // The tooltip to show.
    Tooltip* tooltip;
};
