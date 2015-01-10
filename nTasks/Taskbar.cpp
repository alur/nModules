/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.cpp
 *  The nModules Project
 *
 *  Implementation of the Taskbar class. Handles layout of the taskbar buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "../nShared/LSModule.hpp"
#include "Taskbar.hpp"
#include "../Utilities/Math.h"
#include "../nShared/DWMColorVal.hpp"


/// <summary>
/// Constructor
/// </summary>
Taskbar::Taskbar(LPCTSTR name) : Drawable(name) {
  mThumbnail = new WindowThumbnail(_T("Thumbnail"), mSettings);

  LoadSettings();

  LayoutSettings defaults;
  mLayoutSettings.Load(mSettings, &defaults);

  StateRender<States>::InitData initData;
  initData[States::Base].defaults.brushSettings[State::BrushType::Background].color = std::unique_ptr<IColorVal>(new DWMColorVal());

  mStateRender.Load(initData, mSettings);

  WindowSettings drawableDefaults, windowSettings;
  drawableDefaults.width = (float)GetSystemMetrics(SM_CXSCREEN);
  drawableDefaults.height = 36;
  drawableDefaults.registerWithCore = true;
  windowSettings.Load(mSettings, &drawableDefaults);
    
  mWindow->Initialize(windowSettings, &mStateRender);
  mWindow->Show();
}


/// <summary>
/// Destructor
/// </summary>
Taskbar::~Taskbar() {
  // Remove all buttons
  mButtonList.clear();

  SAFEDELETE(mThumbnail);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Taskbar::LoadSettings(bool /* isRefresh */) {
  Settings *buttonSettings = mSettings->CreateChild(L"Button");

  mButtonSettings.Load(buttonSettings);

  mButtonWidth = buttonSettings->GetDistance(L"Width", 150);
  mButtonHeight = buttonSettings->GetDistance(L"Height", 36);
  mButtonMaxWidth = buttonSettings->GetDistance(L"MaxWidth", mButtonWidth);
  mButtonMaxHeight = buttonSettings->GetDistance(L"MaxHeight", mButtonHeight);
  mMonitor = mSettings->GetMonitor(L"Monitor", 0xFFFFFFFF);
  mNoThumbnails = mSettings->GetBool(L"NoThumbnails", false);

  delete buttonSettings;
}


/// <summary>
/// Adds the specified task to this taskbar
/// </summary>
TaskButton *Taskbar::AddTask(HWND hWnd, UINT monitor, bool noLayout) {
  if (monitor == mMonitor || mMonitor == 0xFFFFFFFF) {
    ASSERT(mButtonMap.find(hWnd) == mButtonMap.end());

    mButtonList.emplace_back(this, hWnd, mButtonSettings);
    mButtonMap[hWnd] = --mButtonList.end();

    if (hWnd == GetForegroundWindow()) {
      mButtonList.back().Activate();
    }

    if (!noLayout) {
      Relayout();
    }

    return &mButtonList.back();
  }

  return nullptr;
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(HWND hWnd) {
  RemoveTask(mButtonMap.find(hWnd));
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(ButtonMap::iterator iter) {
  if (iter != mButtonMap.end()) {
    mButtonList.erase(iter->second);
    mButtonMap.erase(iter);
    Relayout();
    Repaint();
  }
}


/// <summary>
/// Called when the specified taskbar has moved to a different monitor.
/// </summary>
/// <param name="pOut">If a taskbutton was added or removed, the pointer to that button. Otherwise nullptr.</param>
/// <returns>True if the task should be contained on this taskbar. False otherwise.</returns>
bool Taskbar::MonitorChanged(HWND hWnd, UINT monitor, TaskButton **pOut) {
  ButtonMap::iterator iter = mButtonMap.find(hWnd);
  *pOut = nullptr;
  // If we should contain the task
  if (monitor == mMonitor || mMonitor == 0xFFFFFFFF) {
    if (iter == mButtonMap.end()) {
      *pOut = AddTask(hWnd, monitor, false);
    }
    return true;
  } else {
    if (iter != mButtonMap.end()) {
      *pOut = &*iter->second;
      RemoveTask(iter);
    }
    return false;
  }
}


/// <summary>
/// Repaints the taskbar.
/// </summary>
void Taskbar::Repaint() {
  mWindow->Repaint();
}


/// <summary>
/// Repositions/Resizes all buttons.  
/// </summary>
void Taskbar::Relayout() {
  Window::UpdateLock lock(mWindow);
  D2D1_SIZE_F const *size = &mWindow->GetSize();

  float spacePerLine, lines, buttonSize, x0, y0, xdir, ydir;

  if (mButtonMap.size() == 0) {
    return;
  }

  switch (mLayoutSettings.mStartPosition) {
  default:
  case LayoutSettings::StartPosition::TopLeft:
    x0 = (float)mLayoutSettings.mPadding.left;
    y0 = (float)mLayoutSettings.mPadding.top;
    xdir = 1;
    ydir = 1;
    break;

  case LayoutSettings::StartPosition::TopRight:
    x0 = size->width - (float)mLayoutSettings.mPadding.right;
    y0 = (float)mLayoutSettings.mPadding.top;
    xdir = -1;
    ydir = 1;
    break;

  case LayoutSettings::StartPosition::BottomLeft:
    x0 = (float)mLayoutSettings.mPadding.left;
    y0 = size->height - (float)mLayoutSettings.mPadding.bottom;
    xdir = 1;
    ydir = -1;
    break;

  case LayoutSettings::StartPosition::BottomRight:
    x0 = size->width - (float)mLayoutSettings.mPadding.right;
    y0 = size->height - (float)mLayoutSettings.mPadding.bottom;
    xdir = -1;
    ydir = -1;
    break;
  }

  if (mLayoutSettings.mPrimaryDirection == LayoutSettings::Direction::Horizontal) {
    float buttonHeight = mButtonHeight.Evaluate(size->height);
    spacePerLine = size->width - mLayoutSettings.mPadding.left - mLayoutSettings.mPadding.right;
    lines = floorf((size->height + mLayoutSettings.mRowSpacing - mLayoutSettings.mPadding.top - mLayoutSettings.mPadding.bottom) / (mLayoutSettings.mRowSpacing + buttonHeight));
    // We need to consider that buttons can't be split between multiple lines.
    buttonSize = std::min(mButtonMaxWidth.Evaluate(size->width), std::min(spacePerLine * lines / (float)mButtonMap.size(), spacePerLine / ceil(mButtonMap.size() / lines)) - mLayoutSettings.mColumnSpacing);
    if (ydir == -1) {
      y0 -= buttonHeight;
    }
    if (xdir == -1) {
      x0 -= buttonSize;
    }

    float x = x0, y = y0;
    for (TaskButton &button : mButtonList) {
      button.Reposition(x, y, buttonSize, buttonHeight);
      x += xdir*(buttonSize + mLayoutSettings.mColumnSpacing);
      if (x < mLayoutSettings.mPadding.left || x > size->width - mLayoutSettings.mPadding.right - buttonSize + 1.0f) {
        x = x0;
        y += ydir*(buttonHeight + mLayoutSettings.mRowSpacing);
      }
      button.Show();
    }
  } else {
    float buttonWidth = mButtonHeight.Evaluate(size->width);
    spacePerLine = size->height - mLayoutSettings.mPadding.top - mLayoutSettings.mPadding.bottom;
    lines = floorf((size->width + mLayoutSettings.mColumnSpacing - mLayoutSettings.mPadding.left - mLayoutSettings.mPadding.right) / (mLayoutSettings.mColumnSpacing + buttonWidth));
    buttonSize = std::min((float)mButtonMaxHeight.Evaluate(size->height), std::min(spacePerLine * lines / (float)mButtonMap.size(), spacePerLine / ceil(mButtonMap.size() / lines)) - mLayoutSettings.mRowSpacing);
    if (ydir == -1) {
      y0 -= buttonSize;
    }
    if (xdir == -1) {
      x0 -= buttonWidth;
    }

    float x = x0, y = y0;
    for (TaskButton &button : mButtonList) {
      button.Reposition(x, y, buttonWidth, buttonSize);
      y += ydir*(buttonSize + mLayoutSettings.mRowSpacing);
      if (y < mLayoutSettings.mPadding.top || y > size->height - mLayoutSettings.mPadding.bottom - buttonSize + 1.0f) {
        y = y0;
        x += xdir*(buttonWidth + mLayoutSettings.mColumnSpacing);
      }
      button.Show();
    }
  }

  this->Repaint();
}


/// <summary>
/// Handles window messages for this taskbar.
/// </summary>
LRESULT WINAPI Taskbar::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
  switch (message) {
  case Window::WM_SIZECHANGE:
    Relayout();
    return 0;
  }

  mEventHandler->HandleMessage(window, message, wParam, lParam);
  return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// 
/// </summary>
void Taskbar::ShowThumbnail(HWND hwnd, LPRECT position) {
  if (!mNoThumbnails) {
    mThumbnail->Show(hwnd, position);
  }
}


/// <summary>
/// 
/// </summary>
void Taskbar::HideThumbnail() {
  mThumbnail->Hide();
}
