//-------------------------------------------------------------------------------------------------
// /nTaskSwitch/TaskSwitcher.cpp
// The nModules Project
//
// Manages the alt-tab window.
//-------------------------------------------------------------------------------------------------
#include "TaskSwitcher.hpp"

#include "../nCoreCom/Core.h"

#include "../nShared/DWMColorVal.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/Window.hpp"

#include "../Utilities/Common.h"

#include <Shlwapi.h>


extern UINT (WINAPI *DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown);
extern HWND gDesktopWindow;


TaskSwitcher::TaskSwitcher()
  : Drawable(L"nTaskSwitch")
  , mHoveredThumbnail(nullptr)
  , mPeekTimer(0)
  , mPeeking(false)
{
  LoadSettings();
  SetParent(mWindow->GetWindowHandle(), nullptr);
  SetWindowLongPtrW(mWindow->GetWindowHandle(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_COMPOSITED);
  SetWindowPos(mWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


TaskSwitcher::~TaskSwitcher() {
  Hide();
}


void TaskSwitcher::LoadSettings() {
  mWindowsPerRow = mSettings->GetInt(L"WindowsPerRow", 7);
  // LivePreview_ms doesn't seem to have any effect on explorer in Windows 8, but lets use it as
  // the deault value anyway.
  mPeekDelay = mSettings->GetInt(L"PeekDelay", SHRegGetIntW(HKEY_CURRENT_USER,
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AltTab\\LivePreview_ms", 1000));

  const MonitorInfo::Monitor &primaryMonitor = nCore::FetchMonitorInfo().GetMonitor(0);

  mThumbnailSettings.Load(mSettings);

  StateRender<State>::InitData initData;
  initData[State::Base].defaults.brushSettings[::State::BrushType::Background].color = std::unique_ptr<IColorVal>(new DWMColorVal());
  initData[State::Base].defaults.fontSize = 16;
  initData[State::Base].defaults.fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
  initData[State::Base].defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
  initData[State::Base].defaults.brushSettings[::State::BrushType::Text].color = Color::Create(0xFFFFFFFF);
  initData[State::Base].defaults.textOffsetTop = 10;
  mStateRender.Load(initData, mSettings);

  WindowSettings defaults, windowSettings;
  defaults.alwaysOnTop = true;

  // Center in the workarea of the primary monitor.
  defaults.x = float(primaryMonitor.workArea.left + primaryMonitor.workAreaWidth*0.2f);
  defaults.y = 200;
  defaults.width = float(primaryMonitor.workAreaWidth*0.6f);
  defaults.height = 600;
  windowSettings.Load(mSettings, &defaults);

  mWindow->Initialize(windowSettings, &mStateRender);

  LayoutSettings layoutDefaults;
  layoutDefaults.mPadding.left = 20;
  layoutDefaults.mPadding.right = 20;
  layoutDefaults.mPadding.top = 40;
  layoutDefaults.mPadding.bottom = 40;
  layoutDefaults.mRowSpacing = 0;
  layoutDefaults.mColumnSpacing = 0;
  mLayoutSettings.Load(mSettings, &layoutDefaults);

  mTaskSize.width = (mWindow->GetSize().width - mLayoutSettings.mPadding.left - mLayoutSettings.mPadding.right - (mWindowsPerRow - 1) * mLayoutSettings.mColumnSpacing) / mWindowsPerRow;
  mTaskSize.height = mTaskSize.width / 1.5707963f;
}


LRESULT WINAPI TaskSwitcher::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID /* extra */) {
  if (message == WM_KEYUP || message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) {
    Hide();
    return 0;
  } else if (message == WM_TIMER && wParam == mPeekTimer && mWindow->IsVisible()) {
    mPeeking = true;
    Preview(mShownWindows[mSelectedWindow]->mTargetWindow);
    mWindow->ClearCallbackTimer(mPeekTimer);
    mPeekTimer = 0;
    return 0;
  } else if (message == WM_MOUSEMOVE || message == WM_MOUSELEAVE) {
    HoveringOverTask(nullptr);
  }

  return DefWindowProcW(window, message, wParam, lParam);
}


void TaskSwitcher::HandleAltTab() {
  if (mWindow->IsVisible()) {
    UpdateActiveWindow(1);
  } else {
    Show(1);
  }
}


void TaskSwitcher::HandleAltShiftTab() {
  if (mWindow->IsVisible()) {
    UpdateActiveWindow(-1);
  } else {
    Show(-1);
  }
}


void TaskSwitcher::Hide() {
  if (!mWindow->IsVisible()) {
    return;
  }

  if (mPeekTimer != 0) {
    mWindow->ClearCallbackTimer(mPeekTimer);
    mPeekTimer = 0;
  }
  mPeeking = false;
  mWindow->Hide();

  if (mShownWindows.size() != 0) {
    (mHoveredThumbnail ? mHoveredThumbnail : mShownWindows[mSelectedWindow])->Activate();

    for (TaskThumbnail *thumbnail : mShownWindows) {
      delete thumbnail;
    }
    mShownWindows.clear(); 
  }

  DwmpActivateLivePreview(0, nullptr, nullptr, 1);
}


void TaskSwitcher::AddWindow(HWND window) {
  mShownWindows.push_back(new TaskThumbnail(
    this,
    window,
    mLayoutSettings.mPadding.left + mShownWindows.size() % mWindowsPerRow * (mTaskSize.width + mLayoutSettings.mColumnSpacing),
    mLayoutSettings.mPadding.top + (int)mShownWindows.size() / mWindowsPerRow * (mTaskSize.height + mLayoutSettings.mRowSpacing),
    mTaskSize.width,
    mTaskSize.height,
    mThumbnailSettings
  ));
}


void TaskSwitcher::Show(int delta) {
  Window::UpdateLock updateLock(mWindow);

  mHoveredThumbnail = nullptr;
  mSelectedWindow = 0;
  mPeeking = false;

  SetActiveWindow(mWindow->GetWindowHandle());
  SetForegroundWindow(mWindow->GetWindowHandle());
  EnumDesktopWindows(nullptr, LoadWindowsCallback, (LPARAM)this);

  if (gDesktopWindow) {
    AddWindow(gDesktopWindow);
  }

  float height = mLayoutSettings.mPadding.top + mLayoutSettings.mPadding.bottom + ((int)mShownWindows.size() - 1) / mWindowsPerRow * (mTaskSize.height + mLayoutSettings.mRowSpacing) + mTaskSize.height;

  const MonitorInfo::Monitor &primaryMonitor = nCore::FetchMonitorInfo().GetMonitor(0);
  mWindow->SetPosition(
    mWindow->GetDrawingSettings()->x,
    primaryMonitor.workAreaHeight / 2.0f - height / 2.0f + (float)primaryMonitor.workArea.top,
    mWindow->GetDrawingSettings()->width, 
    height);

  mWindow->Show(SW_SHOWNORMAL);
    
  for (auto &thumbnail : mShownWindows) {
    thumbnail->UpdateIconPosition();
  }

  mPeekTimer = mWindow->SetCallbackTimer(mPeekDelay, this);
    
  UpdateActiveWindow(delta);
}


/// <summary>
/// Worker used by AddExisting.
/// </summary>
BOOL CALLBACK TaskSwitcher::LoadWindowsCallback(HWND window, LPARAM taskSwitcher) {
  if (IsTaskbarWindow(window)) {
    ((TaskSwitcher*)taskSwitcher)->AddWindow(window);
  }
  return TRUE;
}


/// <summary>
/// Determines if a window should be shown on the taskbar.
/// </summary>
bool TaskSwitcher::IsTaskbarWindow(HWND hWnd) {
  // Make sure it's actually a window.
  if (!IsWindow(hWnd))
    return false;

  // And that it's visible
  if (!IsWindowVisible(hWnd))
    return false;

  LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

  // Windows with the WS_EX_APPWINDOW style should always be shown
  if ((exStyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW)
    return true;
  else if (GetParent(hWnd) != nullptr) // Windows with parents should not be shown
    return false;
  else if (::GetWindow(hWnd, GW_OWNER) != nullptr) // Windows with owners should not be shown
    return false;
  else if ((exStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) // Tool windows should not be shown on the taskbar
    return false;

  return true;
}


/// <summary>
/// 
/// </summary>
void TaskSwitcher::UpdateActiveWindow(int delta) {
  if (mShownWindows.size() == 0) {
    return;
  }

  if (delta != 0) {
    mShownWindows[mSelectedWindow]->Deselect();

    mSelectedWindow += delta;

    if (mSelectedWindow < 0) {
      mSelectedWindow = int(mShownWindows.size() - 1);
    } else if (mSelectedWindow >= int(mShownWindows.size())) {
      mSelectedWindow = 0;
    }

    mShownWindows[mSelectedWindow]->Select();
    mHoveredThumbnail = nullptr;
  }

  HWND targetWindow = mHoveredThumbnail ? mHoveredThumbnail->mTargetWindow :
    mShownWindows[mSelectedWindow]->mTargetWindow;

  if (targetWindow == gDesktopWindow) {
    mWindow->SetText(L"Desktop");
  } else {
    WCHAR text[MAX_PATH];
    GetWindowTextW(targetWindow, text, MAX_PATH);
    mWindow->SetText(text);
  }
  mWindow->Repaint();

  Preview(targetWindow);
}


void TaskSwitcher::Preview(HWND window) {
  if (mPeeking) {
    if (window == gDesktopWindow) {
      DwmpActivateLivePreview(1, GetDesktopWindow(), mWindow->GetWindowHandle(), 1);
    } else {
      DwmpActivateLivePreview(1, window, mWindow->GetWindowHandle(), 1);
    }
  } else if (mPeekTimer != 0) {
    SetTimer(mWindow->GetWindowHandle(), mPeekTimer, mPeekDelay, nullptr);
  }
}


void TaskSwitcher::HoveringOverTask(TaskThumbnail *task) {
  if (task != mHoveredThumbnail) {
    mHoveredThumbnail = task;
    UpdateActiveWindow(0);
  }
}
