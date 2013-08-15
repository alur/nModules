/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.cpp
 *  The nModules Project
 *
 *  Implementation of the Tray class. Handles layout of the tray buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "Tray.hpp"
#include "TrayIcon.hpp"
#include "../Utilities/Process.h"
#include "../Utilities/GUID.h"
#include "../Utilities/Math.h"
#include <shellapi.h>
#include <Mmsystem.h>
#include <map>


extern HWND g_hWndTrayNotify;
extern bool g_InitPhase;


/// <summary>
/// Constructor
/// </summary>
Tray::Tray(LPCTSTR name) : Drawable(name)
{
    this->balloonClickedMessage = mWindow->RegisterUserMessage(this);
    this->tooltip = new Tooltip(L"Tooltip", mSettings);
    this->balloon = new Balloon(L"Balloon", mSettings, this->balloonClickedMessage, this);

    WindowSettings defaults;
    defaults.registerWithCore = true;

    mWindow->Initialize(&defaults);
    mWindow->Show();

    this->balloonTimer = 0;

    LoadSettings();

    this->infoIcon = LoadIcon(NULL, IDI_INFORMATION);
    this->warningIcon = LoadIcon(NULL, IDI_WARNING);
    this->errorIcon = LoadIcon(NULL, IDI_ERROR);
    this->activeBalloonIcon = nullptr;
}


/// <summary>
/// Destructor
/// </summary>
Tray::~Tray()
{
    // Remove all icons
    for (TrayIcon *icon : this->icons)
    {
        delete icon;
    }
    this->icons.clear();

    if (this->balloonClickedMessage != 0)
    {
        mWindow->ReleaseUserMessage(this->balloonClickedMessage);
    }

    SAFEDELETE(this->tooltip);
    SAFEDELETE(this->balloon);

    DestroyIcon(this->infoIcon);
    DestroyIcon(this->warningIcon);
    DestroyIcon(this->errorIcon);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Tray::LoadSettings(bool /* IsRefresh */)
{
    LayoutSettings layoutDefaults;
    mLayoutSettings.Load(mSettings, &layoutDefaults);

    Settings* iconSettings = mSettings->CreateChild(_T("Icon"));
    mIconSize = iconSettings->GetInt(_T("Size"), 16);
    delete iconSettings;

    mNoTooltips = mSettings->GetBool(_T("NoTooltips"), false);
    mHideBalloons = mSettings->GetBool(_T("HideBalloons"), false);
    this->balloonTime = mSettings->GetInt(_T("BalloonTime"), 7000);
    this->noNotificationSounds = mSettings->GetBool(_T("NoNotificationSounds"), false);
    mSettings->GetString(_T("NotificationSound"), this->notificationSound, 128, _T("Notification.Default"));

    WindowSettings *drawingSettings = mWindow->GetDrawingSettings();

    mTargetSize = D2D1::SizeU(
        drawingSettings->width,
        drawingSettings->height
    );

    mTargetPosition = D2D1::Point2U(
        drawingSettings->x,
        drawingSettings->y
    );

    mOverflowAction = mSettings->GetEnum<OverflowAction>(_T("OverflowAction"),
    {
        { OverflowAction::None,      _T("None")      },
        { OverflowAction::SizeDown,  _T("SizeDown")  },
        { OverflowAction::SizeLeft,  _T("SizeLeft")  },
        { OverflowAction::SizeRight, _T("SizeRight") },
        { OverflowAction::SizeUp,    _T("SizeUp")    }
    }, OverflowAction::None);

    mSettings->GetLine(_T("OnResize"), mOnResize, _countof(mOnResize), nullptr);

    // Load hidden icons
    TCHAR keyName[MAX_RCCOMMAND];
    StringCchPrintf(keyName, _countof(keyName), _T("*%sHide"), mSettings->GetPrefix());
    LiteStep::IterateOverLines(keyName, [this] (LPCWSTR line) -> void
    {
        // Try to parse it as a GUID, if that fails assume it's a process.
        GUID guid;
        if (GUIDFromStringW(line, &guid) != FALSE)
        {
            mHiddenIconIDs.push_back(IconID(guid));
        }
        else
        {
            mHiddenIconIDs.push_back(IconID(line));
        }
    });
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
TrayIcon* Tray::AddIcon(LiteStep::LPLSNOTIFYICONDATA NID)
{
    if (WantIcon(NID))
    {
        TrayIcon* icon = new TrayIcon(this, NID, mSettings);
        this->icons.push_back(icon);
        icon->Show();
        if (!g_InitPhase)
        {
            Relayout();
        }
        return icon;
    }
    return nullptr;
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
bool Tray::WantIcon(LiteStep::LPLSNOTIFYICONDATA NID)
{
    // We could block/accept based on 4 things.
    // 1. Process name
    // 2. GUID -- if specified
    // 3. Window Class
    // 4. Window Text

    // 1. Process name
    WCHAR processName[MAX_PATH];
    GetProcessName(NID->hWnd, false, processName, _countof(processName));
    
    for (IconID &iconID : mHiddenIconIDs)
    {
        switch (iconID.type)
        {
        case IconID::Type::GUID:
            {
                if ((NID->uFlags & NIF_GUID) == NIF_GUID && NID->guidItem == iconID.guid)
                {
                    return false;
                }
            }
            break;

        case IconID::Type::Process:
            {
                if (_wcsicmp(processName, iconID.process) == 0)
                {
                    return false;
                }
            }
            break;
        }
    }

    return true;
}


/// <summary>
/// Finds the specified icon.
/// </summary>
vector<TrayIcon*>::const_iterator Tray::FindIcon(TrayIcon* pIcon)
{
    for (vector<TrayIcon*>::const_iterator iter = this->icons.begin(); iter != this->icons.end(); iter++)
    {
        if ((*iter) == pIcon)
        {
            return iter;
        }
    }
    return this->icons.end();
}


/// <summary>
/// Removes the specified icon from this tray, if it is in it.
/// </summary>
void Tray::RemoveIcon(TrayIcon* pIcon)
{
    vector<TrayIcon*>::const_iterator icon = FindIcon(pIcon);
    if (icon != this->icons.end())
    {
        this->icons.erase(icon);
        
        if (pIcon == this->activeBalloonIcon)
        {
            DismissBalloon(NIN_BALLOONHIDE);
        }
        delete pIcon;

        Relayout();
    }
}


/// <summary>
/// Repositions/Resizes all icons.
/// </summary>
void Tray::Relayout()
{
    Window::UpdateLock lock(mWindow);

    WindowSettings *drawingSettings = mWindow->GetDrawingSettings();

    switch (mOverflowAction)
    {
    case OverflowAction::SizeRight:
    case OverflowAction::SizeLeft:
        {
            int iconsPerColumn = mLayoutSettings.ItemsPerColumn(mIconSize, drawingSettings->height);
            int requiredColumns = (int)(this->icons.size() + iconsPerColumn - 1) / iconsPerColumn; // ceil(y/x) = floor((y + x - 1)/x)
            int requiredWidth = std::max((mIconSize + mLayoutSettings.mColumnSpacing) * requiredColumns +
                mLayoutSettings.mColumnSpacing + mLayoutSettings.mPadding.left + mLayoutSettings.mPadding.right,
                (long)mTargetSize.width);

            if (drawingSettings->width != requiredWidth)
            {
                mWindow->SetPosition(
                    mOverflowAction == OverflowAction::SizeRight ? mTargetPosition.x : mTargetPosition.x - (requiredWidth - mTargetSize.width),
                    mTargetPosition.y,
                    requiredWidth,
                    mTargetSize.height,
                    1
                );
            }
        }
        break;

    case OverflowAction::SizeUp:
    case OverflowAction::SizeDown:
        int iconsPerRow = mLayoutSettings.ItemsPerRow(mIconSize, drawingSettings->width);
        int requiredRows = (int)(this->icons.size() + iconsPerRow - 1) / iconsPerRow; // ceil(y/x) = floor((y + x - 1)/x)
        int requiredHeight = std::max((mIconSize + mLayoutSettings.mRowSpacing) * requiredRows +
            mLayoutSettings.mRowSpacing + mLayoutSettings.mPadding.top + mLayoutSettings.mPadding.bottom,
            (long)mTargetSize.height);

        if (drawingSettings->height != requiredHeight)
        {
            mWindow->SetPosition(
                mTargetPosition.x,
                mOverflowAction == OverflowAction::SizeDown ? mTargetPosition.y : mTargetPosition.y - (requiredHeight - mTargetSize.height),
                mTargetSize.width,
                requiredHeight,
                1
            );
        }
        break;
    }

    int i = 0;
    for (auto icon : this->icons)
    {
        icon->Reposition(mLayoutSettings.RectFromID(i++, mIconSize, mIconSize, drawingSettings->width, drawingSettings->height));
    }
}


/// <summary>
/// Handles window events for the tray.
/// </summary>
LRESULT WINAPI Tray::HandleMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    mEventHandler->HandleMessage(wnd, message, wParam, lParam);
    switch (message)
    {
    case WM_MOUSEMOVE:
        {
            if (IsWindow(g_hWndTrayNotify))
            {
                RECT r;
                mWindow->GetScreenRect(&r);
                MoveWindow(g_hWndTrayNotify, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
            }
        }
        return 0;

    case WM_TIMER:
        {
            if (this->balloonTimer == wParam)
            {
                ShowNextBalloon();
            }
        }
        return 0;

    case Window::WM_TOPPARENTLOST:
        {
        }
        return 0;

    case Window::WM_NEWTOPPARENT:
        {
        }
        return 0;

    case Window::WM_SIZECHANGE:
        {
            mSettings->SetInt(L"CurrentWidth", LOWORD(wParam));
            mSettings->SetInt(L"CurrentHeight", HIWORD(wParam));
            // lParam is 1 when the size change is due to OverflowAction
            if (lParam == 1)
            {
            }
            LiteStep::LSExecute(nullptr, mOnResize, 0);
        }
        return 0;

    default:
        {
            if (message == this->balloonClickedMessage)
            {
                // wParam is 0 if the dialog was clicked. 1 if the x was clicked.
                if (wParam == 0)
                {
                    this->activeBalloonIcon->SendCallback(NIN_BALLOONUSERCLICK, 0, 0);
                }
                DismissBalloon(NIN_BALLOONHIDE);
            }
        }
        return DefWindowProc(wnd, message, wParam, lParam);
    }
}


/// <summary>
/// Called when the init phase has ended.
/// </summary>
void Tray::InitCompleted()
{
    Relayout();
}


/// <summary>
/// Shows the specified tooltip for this tray.
/// </summary>
void Tray::ShowTip(LPCWSTR text, LPRECT position)
{
    if (!mNoTooltips)
    {
        this->tooltip->Show(text, position);
    }
}


/// <summary>
/// Hides the tooltip for this tray.
/// </summary>
void Tray::HideTip()
{
    this->tooltip->Hide();
}


/// <summary>
/// Enqueues a balloon.
/// </summary>
void Tray::EnqueueBalloon(TrayIcon* icon, LPCWSTR infoTitle, LPCWSTR info, DWORD infoFlags, HICON balloonIcon, bool realTime)
{
    // Get the user notification state.
    QUERY_USER_NOTIFICATION_STATE state;
    SHQueryUserNotificationState(&state);

    // Realtime balloons are discarded unless they can be shown imediately.
    if (mHideBalloons || realTime && (this->balloonTimer != 0 || state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME))
    {
        return;
    }

    if (!mWindow->IsVisible())
    {
        return;
    }

    BalloonData data;
    data.icon = icon;
    data.infoTitle = _wcsdup(infoTitle);
    data.info = _wcsdup(info);
    data.infoFlags = infoFlags;
    data.balloonIcon = balloonIcon;

    this->queuedBalloons.push_back(data);

    if (this->balloonTimer == 0)
    {
        ShowNextBalloon();
    }
}


/// <summary>
/// Dismisses a balloon notification prematurely.
/// </summary>
void Tray::DismissBalloon(UINT message)
{
    // Reset the timer.
    SetTimer(mWindow->GetWindowHandle(), this->balloonTimer, this->balloonTime, NULL);

    this->balloon->Hide();
    this->activeBalloonIcon->SendCallback(message, NULL, NULL);
    this->activeBalloonIcon = NULL;

    ShowNextBalloon();
}


/// <summary>
/// Hides the current balloon and shows the next balloon in the queue.
/// </summary>
void Tray::ShowNextBalloon()
{
    if (this->activeBalloonIcon != NULL)
    {
        this->balloon->Hide();
        this->activeBalloonIcon->SendCallback(NIN_BALLOONTIMEOUT, NULL, NULL);
        this->activeBalloonIcon = NULL;
    }

    // Get the user notification state.
    QUERY_USER_NOTIFICATION_STATE state;
    SHQueryUserNotificationState(&state);
    
    // If we are not accepting notifications at this time, we should wait.
    if (state != 0 && state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME)
    {
        if (this->balloonTimer == 0)
        {
            this->balloonTimer = mWindow->SetCallbackTimer(this->balloonTime, this);
        }
        return;
    }

    // Get the balloon to display.
    BalloonData d;
    // Discard balloons for icons which have gone away, or if we are in quiet mode.
    do
    {
        // If there are no more balloons
        if (this->queuedBalloons.empty())
        {
            mWindow->ClearCallbackTimer(this->balloonTimer);
            this->balloonTimer = 0;

            return;
        }

        d = *(this->queuedBalloons.begin());
        this->queuedBalloons.pop_front();
        
        // TODO::Maybe we should permit balloons for icons which have gone away.
    } while(FindIcon(d.icon) == this->icons.end() || state == QUNS_QUIET_TIME && (d.infoFlags & NIIF_RESPECT_QUIET_TIME) == NIIF_RESPECT_QUIET_TIME);

    //
    SIZE iconSize;
    if ((d.infoFlags & NIIF_LARGE_ICON) == NIIF_LARGE_ICON)
    {
        iconSize.cx = GetSystemMetrics(SM_CXICON);
        iconSize.cy = GetSystemMetrics(SM_CYICON);
    }
    else
    {
        iconSize.cx = GetSystemMetrics(SM_CXSMICON);
        iconSize.cy = GetSystemMetrics(SM_CYSMICON);
    }

    // 
    HICON icon = NULL;
    if ((d.infoFlags & NIIF_INFO) == NIIF_INFO)
    {
        icon = this->infoIcon;
    }
    else if ((d.infoFlags & NIIF_WARNING) == NIIF_WARNING)
    {
        icon = this->warningIcon;
    }
    else if ((d.infoFlags & NIIF_ERROR) == NIIF_ERROR)
    {
        icon = this->errorIcon;
    }
    else if ((d.infoFlags & NIIF_USER ) == NIIF_USER && d.balloonIcon != NULL)
    {
        icon = d.balloonIcon;
    }
    
    if (this->balloonTimer == 0)
    {
        this->balloonTimer = mWindow->SetCallbackTimer(this->balloonTime, this);

        if ((d.infoFlags & NIIF_NOSOUND) != NIIF_NOSOUND && !this->noNotificationSounds)
        {
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
