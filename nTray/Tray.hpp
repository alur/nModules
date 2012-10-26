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
#include "../nShared/Balloon.hpp"

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

    // Hides the trays tooltip.
    void HideTip();

    // Enques a balloon tip for display.
    void EnqueueBalloon(TrayIcon* icon, LPCWSTR infoTitle, LPCWSTR info, DWORD infoFlags, HICON balloonIcon, bool realTime);

private:
    // Hides the current balloon, and possible shows the next balloon.
    void ShowNextBalloon();

    // All data required to display a balloon.
    typedef struct {
        TrayIcon* icon;
        LPCWSTR infoTitle;
        LPCWSTR info;
        DWORD infoFlags;
        HICON balloonIcon;
    } BalloonData;

    // Balloons queued up to be displayed.
    list<BalloonData> queuedBalloons;

    // Loads .rc settings for this tray.
    void LoadSettings(bool isRefresh = false);

    // Positions all tray icons properly.
    void Relayout();

    // Finds the specified icon in the tray.
    vector<TrayIcon*>::const_iterator FindIcon(TrayIcon* icon);

    // The tray icons.
    vector<TrayIcon*> icons;

    // The size of the icons.
    int iconSize;

    // Defines how the tray icons should be layed out.
    LayoutSettings* layoutSettings;

    // The tooltip to show.
    Tooltip* tooltip;

    // The balloon to show.
    Balloon* balloon;

    // True if we should hide balloons.
    bool hideBalloons;

    // Number of milliseconds to show balloons.
    int balloonTime;

    // Fires when we should hide the current balloon, and possibly show the next one.
    UINT_PTR balloonTimer;

    // Standard balloon icons.
    HICON infoIcon, warningIcon, errorIcon;

    //
    bool noNotificationSounds;

    //
    WCHAR notificationSound[128];
};
