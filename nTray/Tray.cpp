//-------------------------------------------------------------------------------------------------
// /nTray/Tray.cpp
// The nModules Project
//
// Implementation of the Tray class. Handles layout of the tray buttons, and renders the tray
// background.
//-------------------------------------------------------------------------------------------------
#include "Tray.hpp"
#include "TrayIcon.hpp"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/GUID.h"
#include "../Utilities/Math.h"
#include "../Utilities/Process.h"

#include <algorithm>
#include <map>
#include <Mmsystem.h>
#include <shellapi.h>

extern HWND ghWndTrayNotify;
extern bool gInitPhase;

// Default settings for labels.
static const WindowSettings sWindowDefaults([] (WindowSettings &defaults) {
  defaults.registerWithCore = true;
});

static const StateRender<TrayIcon::States>::InitData sIconInitData([] (StateRender<TrayIcon::States>::InitData &initData) {
  initData[TrayIcon::States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0x00000000);
});


/// <summary>
/// Constructor
/// </summary>
Tray::Tray(LPCTSTR name)
  : Drawable(name)
  , mBalloon(L"Balloon", mSettings, mWindow->RegisterUserMessage(this), this)
  , mBalloonTimer(0)
  , mHideBalloons(false)
  , mNoTooltips(false)
  , mActiveBalloonIcon(nullptr)
  , mTooltip(L"Tooltip", mSettings)
{
  mOnResize[0] = L'\0';
  mStateRender.Load(mSettings);

  WindowSettings windowSettings;
  windowSettings.Load(mSettings, &sWindowDefaults);

  mWindow->Initialize(windowSettings, &mStateRender);
  mWindow->Show();

  LoadSettings();

  mInfoIcon = LoadIcon(nullptr, IDI_INFORMATION);
  mWarningIcon = LoadIcon(nullptr, IDI_WARNING);
  mErrorIcon = LoadIcon(nullptr, IDI_ERROR);
}


/// <summary>
/// Destructor
/// </summary>
Tray::~Tray() {
  // Remove all icons
  for (TrayIcon *icon : mIcons) {
    delete icon;
  }

  if (mBalloon.GetClickedMessage() != 0) {
    mWindow->ReleaseUserMessage(mBalloon.GetClickedMessage());
  }

  DestroyIcon(mInfoIcon);
  DestroyIcon(mWarningIcon);
  DestroyIcon(mErrorIcon);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Tray::LoadSettings() {
  mIconLayout.Load(mSettings);

  mNoTooltips = mSettings->GetBool(L"NoTooltips", false);
  mHideBalloons = mSettings->GetBool(L"HideBalloons", false);
  mBalloonTime = mSettings->GetInt(L"BalloonTime", 7000);
  mNoNotificationSounds = mSettings->GetBool(L"NoNotificationSounds", false);
  mSettings->GetString(L"NotificationSound", mNotificationSound, _countof(mNotificationSound),
    L"Notification.Default");

  WindowSettings *drawingSettings = mWindow->GetDrawingSettings();

  // TODO::FIX ME!
  mTargetSize = D2D1::SizeF(
    drawingSettings->width.Evaluate(0),
    drawingSettings->height.Evaluate(0)
  );

  mTargetPosition = D2D1::Point2F(
    drawingSettings->x.Evaluate(0),
    drawingSettings->y.Evaluate(0)
  );

  mOverflowAction = mSettings->GetEnum<OverflowAction>(L"OverflowAction", {
    { OverflowAction::None,      L"None"      },
    { OverflowAction::SizeDown,  L"SizeDown"  },
    { OverflowAction::SizeLeft,  L"SizeLeft"  },
    { OverflowAction::SizeRight, L"SizeRight" },
    { OverflowAction::SizeUp,    L"SizeUp"    }
  }, OverflowAction::None);

  mSettings->GetLine(L"OnResize", mOnResize, _countof(mOnResize), nullptr);

  mSettings->IterateOverCommandLines(L"Hide", [this] (LPCWSTR line) -> void {
    // Try to parse it as a GUID, if that fails assume it's a process.
    GUID guid;
    if (GUIDFromStringW(line, &guid) != FALSE) {
      mIconBlacklist.emplace_front(guid);
    } else {
      mIconBlacklist.emplace_front(line);
    }
  });

  // Load icon settings
  Settings *iconSettings = mSettings->CreateChild(L"Icon");
  mIconSize = iconSettings->GetInt(L"Size", 16);
  mIconStates.Load(sIconInitData, iconSettings);
  mIconWindowSettings.Load(iconSettings);
  delete iconSettings;
}


/// <summary>
/// Adds the specified icon to this tray.
/// </summary>
TrayIcon *Tray::AddIcon(IconData &iconData) {
  if (!WantIcon(iconData)) {
    return nullptr;
  }
  TrayIcon *icon = new TrayIcon(this, iconData, mIconWindowSettings, &mIconStates);
  mIcons.push_back(icon);
  if (!gInitPhase) {
    icon->Show();
    Relayout();
  }
  return icon;
}


/// <summary>
/// Returns true if this tray wants the specified icon.
/// </summary>
bool Tray::WantIcon(IconData &iconData) {
  // We could block/accept based on 4 things.
  // 1. Process name
  // 2. GUID -- if specified
  // 3. Window Class
  // 4. Window Text

  // 1. Process name
  WCHAR processName[MAX_PATH];
  GetProcessName(iconData.window, false, processName, _countof(processName));

  for (IconId &iconId : mIconBlacklist) {
    switch (iconId.type) {
    case IconId::Type::GUID:
      if ((iconData.flags & NIF_GUID) == NIF_GUID && iconData.guid == iconId.guid) {
        return false;
      }
      break;

    case IconId::Type::Process:
      if (_wcsicmp(processName, iconId.process) == 0) {
        return false;
      }
      break;
    }
  }

  return true;
}


/// <summary>
/// Finds the specified icon.
/// </summary>
vector<TrayIcon*>::iterator Tray::FindIcon(TrayIcon *icon) {
  return std::find(mIcons.begin(), mIcons.end(), icon);
}


/// <summary>
/// Removes the specified icon from this tray, if it is in it.
/// </summary>
void Tray::RemoveIcon(TrayIcon *pIcon) {
  vector<TrayIcon*>::const_iterator icon = FindIcon(pIcon);
  if (icon != mIcons.end()) {
    mIcons.erase(icon);

    if (pIcon == mActiveBalloonIcon) {
      DismissBalloon(NIN_BALLOONHIDE);
    }
    delete pIcon;

    Relayout();
  }
}


/// <summary>
/// Repositions/Resizes all icons.
/// </summary>
void Tray::Relayout() {
  Window::UpdateLock lock(mWindow);
  D2D1_SIZE_F const &size = mWindow->GetSize();

  switch (mOverflowAction) {
  case OverflowAction::SizeRight:
  case OverflowAction::SizeLeft: {
    int iconsPerColumn = mIconLayout.ItemsPerColumn(mIconSize, (int)size.height);
    int requiredColumns = (int)(mIcons.size() + iconsPerColumn - 1) / iconsPerColumn; // ceil(y/x) = floor((y + x - 1)/x)
    int requiredWidth = std::max((mIconSize + mIconLayout.mColumnSpacing) * requiredColumns -
      mIconLayout.mColumnSpacing + mIconLayout.mPadding.left + mIconLayout.mPadding.right,
      (long)mTargetSize.width);

    int i = 0;
    for (auto icon : mIcons) {
      icon->Reposition(mIconLayout.RectFromID(i++, mIconSize, mIconSize, requiredWidth, (int)mTargetSize.height));
    }

    if (size.width != requiredWidth) {
      mWindow->SetPosition(
        float(mOverflowAction == OverflowAction::SizeRight ? mTargetPosition.x : mTargetPosition.x - (requiredWidth - mTargetSize.width)),
        mTargetPosition.y,
        (float)requiredWidth,
        mTargetSize.height,
        1
        );
      }
    }
    break;

  case OverflowAction::SizeUp:
  case OverflowAction::SizeDown: {
    int iconsPerRow = mIconLayout.ItemsPerRow(mIconSize, (int)size.width);
    int requiredRows = (int)(mIcons.size() + iconsPerRow - 1) / iconsPerRow; // ceil(y/x) = floor((y + x - 1)/x)
    int requiredHeight = std::max((mIconSize + mIconLayout.mRowSpacing) * requiredRows -
      mIconLayout.mRowSpacing + mIconLayout.mPadding.top + mIconLayout.mPadding.bottom,
      (long)mTargetSize.height);

    int i = 0;
    for (auto icon : mIcons) {
      icon->Reposition(mIconLayout.RectFromID(i++, mIconSize, mIconSize, (int)mTargetSize.width, requiredHeight));
    }

    if (size.height != requiredHeight) {
      mWindow->SetPosition(
        mTargetPosition.x,
        float(mOverflowAction == OverflowAction::SizeDown ? mTargetPosition.y : mTargetPosition.y - (requiredHeight - mTargetSize.height)),
        mTargetSize.width,
        (float)requiredHeight,
        1
        );
      }
    }
    break;
  }
}


/// <summary>
/// Handles window events for the tray.
/// </summary>
LRESULT WINAPI Tray::HandleMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
  mEventHandler->HandleMessage(wnd, message, wParam, lParam);
  switch (message) {
  case WM_MOUSEMOVE:
    if (IsWindow(ghWndTrayNotify)) {
      RECT r;
      mWindow->GetScreenRect(&r);
      MoveWindow(ghWndTrayNotify, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
    }
    return 0;

  case WM_TIMER:
    if (mBalloonTimer == wParam) {
      ShowNextBalloon();
    }
    return 0;

  case Window::WM_TOPPARENTLOST:
    mBalloon.SetClickedMessage(0);
    return 0;

  case Window::WM_NEWTOPPARENT:
    mBalloon.SetClickedMessage(mWindow->RegisterUserMessage(this));
    return 0;

  case Window::WM_SIZECHANGE:
    mSettings->SetInt(L"CurrentWidth", LOWORD(wParam));
    mSettings->SetInt(L"CurrentHeight", HIWORD(wParam));
    // lParam is 1 when the size change is due to OverflowAction
    if (lParam == 1) {
      LiteStep::LSExecute(nullptr, mOnResize, 0);
    }
    return 0;

  default:
    if (message == mBalloon.GetClickedMessage()) {
      // wParam is 0 if the dialog was clicked. 1 if the x was clicked.
      if (wParam == 0) {
        mActiveBalloonIcon->SendCallback(NIN_BALLOONUSERCLICK, 0, 0);
      }
      DismissBalloon(NIN_BALLOONHIDE);
    }
    return DefWindowProc(wnd, message, wParam, lParam);
  }
}


/// <summary>
/// Called when the init phase has ended.
/// </summary>
void Tray::InitCompleted() {
  Window::UpdateLock lock(mWindow);
  Relayout();
  for (TrayIcon *icon : mIcons) {
    icon->Show();
  }
}


/// <summary>
/// Shows the specified tooltip for this tray.
/// </summary>
void Tray::ShowTip(LPCWSTR text, LPRECT position) {
  if (!mNoTooltips) {
    mTooltip.Show(text, position);
  }
}


/// <summary>
/// Hides the tooltip for this tray.
/// </summary>
void Tray::HideTip() {
  mTooltip.Hide();
}


/// <summary>
/// Enqueues a balloon.
/// </summary>
void Tray::EnqueueBalloon(TrayIcon *icon, LPCWSTR infoTitle, LPCWSTR info, DWORD infoFlags,
  HICON balloonIcon, bool realTime)
{
  // Get the user notification state.
  QUERY_USER_NOTIFICATION_STATE state;
  SHQueryUserNotificationState(&state);

  // Realtime balloons are discarded unless they can be shown imediately.
  if (mHideBalloons || realTime &&
    (mBalloonTimer != 0 || state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME))
  {
    return;
  }

  if (!mWindow->IsVisible()) {
    return;
  }

  BalloonData data;
  data.icon = icon;
  data.infoTitle = _wcsdup(infoTitle);
  data.info = _wcsdup(info);
  data.infoFlags = infoFlags;
  data.balloonIcon = balloonIcon;

  mQueuedBalloons.push_back(data);

  if (mBalloonTimer == 0) {
    ShowNextBalloon();
  }
}


/// <summary>
/// Dismisses a balloon notification prematurely.
/// </summary>
void Tray::DismissBalloon(UINT message) {
  // Reset the timer.
  SetTimer(mWindow->GetWindowHandle(), mBalloonTimer, mBalloonTime, nullptr);

  mBalloon.Hide();
  mActiveBalloonIcon->SendCallback(message, 0, 0);
  mActiveBalloonIcon = nullptr;

  ShowNextBalloon();
}


/// <summary>
/// Hides the current balloon and shows the next balloon in the queue.
/// </summary>
void Tray::ShowNextBalloon() {
  if (mActiveBalloonIcon != nullptr) {
    mBalloon.Hide();
    mActiveBalloonIcon->SendCallback(NIN_BALLOONTIMEOUT, 0, 0);
    mActiveBalloonIcon = nullptr;
  }

  // Get the user notification state.
  QUERY_USER_NOTIFICATION_STATE state;
  SHQueryUserNotificationState(&state);

  // If we are not accepting notifications at this time, we should wait.
  if (state != 0 && state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_QUIET_TIME) {
    if (mBalloonTimer == 0) {
      mBalloonTimer = mWindow->SetCallbackTimer(mBalloonTime, this);
    }
    return;
  }

  // Get the balloon to display.
  BalloonData d;
  // Discard balloons for icons which have gone away, or if we are in quiet mode.
  do {
    // If there are no more balloons
    if (mQueuedBalloons.empty()) {
      mWindow->ClearCallbackTimer(mBalloonTimer);
      mBalloonTimer = 0;
      return;
    }

    d = *mQueuedBalloons.begin();
    mQueuedBalloons.pop_front();

    // TODO::Maybe we should permit balloons for icons which have gone away.
  } while(FindIcon(d.icon) == mIcons.end() || state == QUNS_QUIET_TIME &&
    (d.infoFlags & NIIF_RESPECT_QUIET_TIME) == NIIF_RESPECT_QUIET_TIME);

  SIZE iconSize;
  if ((d.infoFlags & NIIF_LARGE_ICON) == NIIF_LARGE_ICON) {
    iconSize.cx = GetSystemMetrics(SM_CXICON);
    iconSize.cy = GetSystemMetrics(SM_CYICON);
  } else {
    iconSize.cx = GetSystemMetrics(SM_CXSMICON);
    iconSize.cy = GetSystemMetrics(SM_CYSMICON);
  }

  HICON icon = nullptr;
  if ((d.infoFlags & 0x3) == NIIF_INFO) {
    icon = mInfoIcon;
  } else if ((d.infoFlags & 0x3) == NIIF_WARNING) {
    icon = mWarningIcon;
  } else if ((d.infoFlags & 0x3) == NIIF_ERROR) {
    icon = mErrorIcon;
  } else if ((d.infoFlags & NIIF_USER) == NIIF_USER && d.balloonIcon != nullptr) {
    icon = d.balloonIcon;
  }

  if (mBalloonTimer == 0) {
    mBalloonTimer = mWindow->SetCallbackTimer(mBalloonTime, this);
    if ((d.infoFlags & NIIF_NOSOUND) != NIIF_NOSOUND && !mNoNotificationSounds) {
      PlaySoundW(mNotificationSound, nullptr, SND_ALIAS | SND_ASYNC | SND_SYSTEM | SND_NODEFAULT);
    }
  }

  //
  RECT targetPosition;
  d.icon->GetScreenRect(&targetPosition);

  mActiveBalloonIcon = d.icon;

  mBalloon.Show(d.infoTitle, d.info, icon, &iconSize, &targetPosition);

  // Free memory
  free((LPVOID)d.infoTitle);
  free((LPVOID)d.info);
}
