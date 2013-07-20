/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.cpp
 *  The nModules Project
 *
 *  Implementation of the Tray class. Handles layout of the tray buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nShared/LSModule.hpp"
#include "Tray.hpp"
#include "TrayIcon.hpp"


extern HWND g_hWndTrayNotify;
extern bool g_InitPhase;


/// <summary>
/// Constructor
/// </summary>
Tray::Tray(LPCSTR name) : Drawable(name) {
    this->balloonClickedMessage = this->window->RegisterUserMessage(this);
    this->tooltip = new Tooltip("Tooltip", this->settings);
    this->balloon = new Balloon("Balloon", this->settings, this->balloonClickedMessage, this);
    this->layoutSettings = new LayoutSettings();

    this->window->Initialize();
    this->window->Show();

    this->balloonTimer = 0;

    LoadSettings();

    this->infoIcon = LoadIcon(NULL, IDI_INFORMATION);
    this->warningIcon = LoadIcon(NULL, IDI_WARNING);
    this->errorIcon = LoadIcon(NULL, IDI_ERROR);
    this->activeBalloonIcon = NULL;
}


/// <summary>
/// Destructor
/// </summary>
Tray::~Tray() {
    // Remove all icons
    for (TrayIcon *icon : this->icons) {
        delete icon;
    }
    this->icons.clear();

    if (this->balloonClickedMessage != 0) {
        this->window->ReleaseUserMessage(this->balloonClickedMessage);
    }

    SAFEDELETE(this->tooltip);
    SAFEDELETE(this->balloon);
    SAFEDELETE(this->layoutSettings);

    DestroyIcon(this->infoIcon);
    DestroyIcon(this->warningIcon);
    DestroyIcon(this->errorIcon);
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

    this->hideBalloons = this->settings->GetBool("HideBalloons", false);
    this->balloonTime = this->settings->GetInt("BalloonTime", 7000);
    this->noNotificationSounds = this->settings->GetBool("NoNotificationSounds", false);
    this->settings->GetString("NotificationSound", this->notificationSound, 128, "Notification.Default");
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
TrayIcon* Tray::AddIcon(LiteStep::LPLSNOTIFYICONDATA NID) {
    TrayIcon* icon = new TrayIcon(this, NID, this->settings);
    this->icons.push_back(icon);
    Relayout();
    icon->Show();
    if (!g_InitPhase) {
        this->window->Repaint();
    }
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
        this->icons.erase(icon);
        
        if (pIcon == this->activeBalloonIcon) {
            DismissBalloon(NIN_BALLOONHIDE);
        }
        delete pIcon;

        Relayout();
        this->window->Repaint();
    }
}


/// <summary>
/// Repositions/Resizes all icons.
/// </summary>
void Tray::Relayout() {
    int i = 0;
    DrawableSettings *drawingSettings = this->window->GetDrawingSettings();

    for (auto icon : this->icons) {
        icon->Reposition(this->layoutSettings->RectFromID(i++, this->iconSize, this->iconSize, drawingSettings->width, drawingSettings->height));
    }
}


/// <summary>
/// Handles window events for the tray.
/// </summary>
LRESULT WINAPI Tray::HandleMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    this->eventHandler->HandleMessage(wnd, message, wParam, lParam);
    switch (message) {
    case WM_MOUSEMOVE:
        {
            if (IsWindow(g_hWndTrayNotify)) {
                RECT r;
                this->window->GetScreenRect(&r);
                MoveWindow(g_hWndTrayNotify, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
            }
        }
        return 0;

    case WM_TIMER:
        {
            if (this->balloonTimer == wParam) {
                ShowNextBalloon();
            }
        }
        return 0;

    case DrawableWindow::WM_TOPPARENTLOST:
        {
        }
        return 0;

    case DrawableWindow::WM_NEWTOPPARENT:
        {
        }
        return 0;

    default:
        {
            if (message == this->balloonClickedMessage) {
                // wParam is NULL if the dialog was clicked. 1 if the x was clicked.
                if (wParam == NULL) {
                    this->activeBalloonIcon->SendCallback(NIN_BALLOONUSERCLICK, NULL, NULL);
                }
                DismissBalloon(NIN_BALLOONHIDE);
            }
        }
        return DefWindowProc(wnd, message, wParam, lParam);
    }
}


/// <summary>
/// Called when the init phase has ended.
/// <summary>
void Tray::InitCompleted() {
    this->window->Repaint();
}


/// <summary>
/// Shows the specified tooltip for this tray.
/// <summary>
void Tray::ShowTip(LPCWSTR text, LPRECT position) {
    this->tooltip->Show(text, position);
}


/// <summary>
/// Hides the tooltip for this tray.
/// <summary>
void Tray::HideTip() {
    this->tooltip->Hide();
}


/// <summary>
/// Enqueues a balloon.
/// <summary>
void Tray::EnqueueBalloon(TrayIcon* icon, LPCWSTR infoTitle, LPCWSTR info, DWORD infoFlags, HICON balloonIcon, bool realTime) {
    // Get the user notification state.
    QUERY_USER_NOTIFICATION_STATE state;
    SHQueryUserNotificationState(&state);

    // Realtime balloons are discarded unless they can be shown imediately.
    if (this->hideBalloons || realTime && (this->balloonTimer != 0 || state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME)) {
        return;
    }

    if (!this->window->IsVisible()) {
        return;
    }

    BalloonData data;
    data.icon = icon;
    data.infoTitle = _wcsdup(infoTitle);
    data.info = _wcsdup(info);
    data.infoFlags = infoFlags;
    data.balloonIcon = balloonIcon;

    this->queuedBalloons.push_back(data);

    if (this->balloonTimer == 0) {
        ShowNextBalloon();
    }
}


/// <summary>
/// Dismisses a balloon notification prematurely.
/// <summary>
void Tray::DismissBalloon(UINT message) {
    // Reset the timer.
    SetTimer(this->window->GetWindowHandle(), this->balloonTimer, this->balloonTime, NULL);

    this->balloon->Hide();
    this->activeBalloonIcon->SendCallback(message, NULL, NULL);
    this->activeBalloonIcon = NULL;

    ShowNextBalloon();
}


/// <summary>
/// Hides the current balloon and shows the next balloon in the queue.
/// <summary>
void Tray::ShowNextBalloon() {
    if (this->activeBalloonIcon != NULL) {
        this->balloon->Hide();
        this->activeBalloonIcon->SendCallback(NIN_BALLOONTIMEOUT, NULL, NULL);
        this->activeBalloonIcon = NULL;
    }

    // Get the user notification state.
    QUERY_USER_NOTIFICATION_STATE state;
    SHQueryUserNotificationState(&state);
    
    // If we are not accepting notifications at this time, we should wait.
    if (state != 0 && state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME) {
        if (this->balloonTimer == 0) {
            this->balloonTimer = this->window->SetCallbackTimer(this->balloonTime, this);
        }
        return;
    }

    // Get the balloon to display.
    BalloonData d;
    // Discard balloons for icons which have gone away, or if we are in quiet mode.
    do {
        // If there are no more balloons
        if (this->queuedBalloons.empty()) {
            this->window->ClearCallbackTimer(this->balloonTimer);
            this->balloonTimer = 0;

            return;
        }

        d = *(this->queuedBalloons.begin());
        this->queuedBalloons.pop_front();
        
        // TODO::Maybe we should permit balloons for icons which have gone away.
    } while(FindIcon(d.icon) == this->icons.end() || state == QUNS_QUIET_TIME && (d.infoFlags & NIIF_RESPECT_QUIET_TIME) == NIIF_RESPECT_QUIET_TIME);

    //
    SIZE iconSize;
    if ((d.infoFlags & NIIF_LARGE_ICON) == NIIF_LARGE_ICON) {
        iconSize.cx = GetSystemMetrics(SM_CXICON);
        iconSize.cy = GetSystemMetrics(SM_CYICON);
    }
    else {
        iconSize.cx = GetSystemMetrics(SM_CXSMICON);
        iconSize.cy = GetSystemMetrics(SM_CYSMICON);
    }

    // 
    HICON icon = NULL;
    if ((d.infoFlags & NIIF_INFO) == NIIF_INFO) {
        icon = this->infoIcon;
    }
    else if ((d.infoFlags & NIIF_WARNING) == NIIF_WARNING) {
        icon = this->warningIcon;
    }
    else if ((d.infoFlags & NIIF_ERROR) == NIIF_ERROR) {
        icon = this->errorIcon;
    }
    else if ((d.infoFlags & NIIF_USER ) == NIIF_USER && d.balloonIcon != NULL) {
        icon = d.balloonIcon;
    }
    
    if (this->balloonTimer == 0) {
        this->balloonTimer = this->window->SetCallbackTimer(this->balloonTime, this);

        if ((d.infoFlags & NIIF_NOSOUND) != NIIF_NOSOUND && !this->noNotificationSounds) {
            PlaySoundW(this->notificationSound, NULL, SND_ALIAS | SND_ASYNC | SND_SYSTEM | SND_NODEFAULT);
        }
    }

    //
    RECT targetPosition;
    d.icon->GetScreenRect(&targetPosition);

    this->activeBalloonIcon = d.icon;

    this->balloon->Show(d.infoTitle, d.info, icon, &iconSize, &targetPosition);

    // Free memory
    free((LPVOID)d.infoTitle);
    free((LPVOID)d.info);
}
