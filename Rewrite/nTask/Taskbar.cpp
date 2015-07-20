#include "Taskbar.hpp"

#include "../nShared/StringMap.hpp"

#include "../nCoreApi/Core.h"

#include "../Headers/lsapi.h"

#include <assert.h>
#include <algorithm>

// TODO(Erik):Simplify this
StateDefinition gButtonStates[] = {
  { L"Minimized",               0x0000, 0 }, // 1
  { L"Flashing",                0x0000, 0 }, // 2
  { L"MinimizedFlashing",       0x0003, 2 }, // 3
  { L"Active",                  0x0000, 0 }, // 4
  { L"Hover",                   0x0000, 0 }, // 5
  { L"MinimizedHover",          0x0011, 5 }, // 6
  { L"ActiveHover",             0x0018, 4 }, // 7
  { L"FlashingHover",           0x0012, 5 }, // 8
  { L"MinimizedFlashingHover",  0x0013, 8 }  // 9
};

BYTE gNumButtonStates = _countof(gButtonStates);

static const StringMap sDefaults({
  { L"ButtonFlashingColor", L"0xAAFF0000" },
  { L"Width", L"100%" },
});


Taskbar::Taskbar(LPCWSTR prefix)
  : mPane(nullptr)
  , mBackgroundPainter(nullptr)
  , mButtonTextPainter(nullptr)
  , mButtonBackgroundPainter(nullptr)
  , mEventHandler(nullptr)
  , mLayoutOnUnlock(false)
  , mLock(1)
{
  mReplacementPosition = mButtons.end();

  ISettingsReader *reader = nCore::CreateSettingsReader(prefix, &sDefaults);
  mEventHandler = nCore::CreateEventHandler(reader);
  mBackgroundPainter = nCore::CreateBackgroundPainter(reader, nullptr, 0);

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.name = prefix;
  initData.settingsReader = reader;
  initData.messageHandler = (IMessageHandler*)this;
  IPainter *painters[] = { mBackgroundPainter, nCore::GetChildPainter() };
  initData.painters = painters;
  initData.numPainters = _countof(painters);
  mPane = nCore::CreatePane(&initData);

  mMonitor = reader->GetMonitor(L"Monitor", MONITOR_ALL);

  ISettingsReader *buttonReader = reader->CreateChild(L"Button");
  mButtonEventHandler = nCore::CreateEventHandler(buttonReader);
  mButtonBackgroundPainter = nCore::CreateBackgroundPainter(buttonReader, gButtonStates,
    gNumButtonStates);
  mButtonTextPainter = nCore::CreateTextPainter(buttonReader, gButtonStates, gNumButtonStates);

  mButtonWidth = buttonReader->GetLength(L"Width", NLENGTH(0, 0, 150));
  mButtonHeight = buttonReader->GetLength(L"Height", NLENGTH(0, 0, 36));
  mButtonMaxWidth = buttonReader->GetLength(L"MaxWidth", mButtonWidth);
  mButtonMaxHeight = buttonReader->GetLength(L"MaxHeight", mButtonHeight);

  buttonReader->Discard();

  mLayoutSettings.Load(reader);

  reader->Discard();
}


Taskbar::~Taskbar() {
  mPane->Lock(); // Never paint again.
  
  mButtonMap.clear();
  mButtons.clear();
  mButtonBackgroundPainter->Discard();
  mButtonTextPainter->Discard();
  mButtonEventHandler->Discard();

  mPane->Discard();
  mBackgroundPainter->Discard();
  mEventHandler->Discard();
}


void Taskbar::Initialized() {
  --mLock;
  Relayout();
  mPane->Show();
}


// Since we can't exactly simplify the division of 2 linear equations, we work with pixels here
// and rerun this whenever the taskbar is resized.
void Taskbar::Relayout() {
  if (mLock != 0 || mButtons.size() == 0) {
    mLayoutOnUnlock = true;
    return;
  }
  mLayoutOnUnlock = false;

  mPane->Lock();

  float x0 = 0, y0 = 0, xdir = 0, ydir = 0;
  D2D1_RECT_F padding = mPane->EvaluateRect(mLayoutSettings.mPadding);
  D2D1_SIZE_F size = mPane->GetRenderingSize();
  D2D1_SIZE_F contentSize = D2D1::SizeF(
    size.width - padding.left - padding.right,
    size.height - padding.top - padding.bottom);

  switch (mLayoutSettings.mStartPosition) {
  case LayoutSettings::StartPosition::TopLeft:
    x0 = padding.left;
    y0 = padding.top;
    xdir = 1;
    ydir = 1;
    break;

  case LayoutSettings::StartPosition::TopRight:
    x0 = size.width - padding.right;
    y0 = padding.top;
    xdir = -1;
    ydir = 1;
    break;

  case LayoutSettings::StartPosition::BottomLeft:
    x0 = padding.left;
    y0 = size.height - padding.bottom;
    xdir = 1;
    ydir = -1;
    break;

  case LayoutSettings::StartPosition::BottomRight:
    x0 = size.width - padding.right;
    y0 = size.height - padding.bottom;
    xdir = -1;
    ydir = -1;
    break;

  default:
    assert(false); // If we change the enum we should update this function.
  }

  float rowSpacing = mPane->EvaluateLength(mLayoutSettings.mRowSpacing, false);
  float colSpacing = mPane->EvaluateLength(mLayoutSettings.mColumnSpacing, true);
  float buttonCount = (float)mButtons.size();
  if (mLayoutSettings.mPrimaryDirection == LayoutSettings::Direction::Horizontal) {
    float buttonHeight = mPane->EvaluateLength(mButtonHeight, false);
    // We need to consider that buttons can't be split between multiple lines.
    float maxLines = floorf((contentSize.height + rowSpacing) / (buttonHeight + rowSpacing));
    float buttonSize = std::min(
      mPane->EvaluateLength(mButtonMaxWidth, true),
      (contentSize.width * maxLines - colSpacing * (buttonCount - maxLines)) / buttonCount);
    if (ydir == -1) {
      y0 -= buttonHeight;
    }
    if (xdir == -1) {
      x0 -= buttonSize;
    }

    float x = x0, y = y0;
    for (TaskButton &button : mButtons) {
      button.Position(NRECT(
        NLENGTH(x, 0, 0),
        NLENGTH(y, 0, 0),
        NLENGTH(x + buttonSize, 0, 0),
        NLENGTH(y + buttonHeight, 0, 0)));
      x += xdir*(buttonSize + colSpacing);
      if (x < padding.left || x > size.width - padding.right - buttonSize + 1.0f) {
        x = x0;
        y += ydir*(buttonHeight + rowSpacing);
      }
    }
  } else {
    float buttonWidth = mPane->EvaluateLength(mButtonWidth, true);
    float maxLines = floorf((contentSize.width + colSpacing) / (buttonWidth + colSpacing));
    float buttonSize = std::min(
      mPane->EvaluateLength(mButtonMaxHeight, false),
      (contentSize.height * maxLines - rowSpacing * (buttonCount - maxLines)) / buttonCount);
    if (ydir == -1) {
      y0 -= buttonSize;
    }
    if (xdir == -1) {
      x0 -= buttonWidth;
    }

    float x = x0, y = y0;
    for (TaskButton &button : mButtons) {
      button.Position(NRECT(
        NLENGTH(x, 0, 0),
        NLENGTH(y, 0, 0),
        NLENGTH(x + buttonWidth, 0, 0),
        NLENGTH(y + buttonSize, 0, 0)));
      y += ydir*(buttonSize + rowSpacing);
      if (y < padding.top || y > size.height - padding.bottom - buttonSize + 1.0f) {
        y = y0;
        x += xdir*(buttonWidth + colSpacing);
      }
    }
  }

  mPane->Unlock();
}


LRESULT Taskbar::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  return mEventHandler->HandleMessage(window, msg, wParam, lParam, nullptr);
};


void Taskbar::ActiveWindowChanged(HWND oldWindow, HWND newWindow) {
  mPane->Lock();

  auto iter = mButtonMap.find(oldWindow);
  if (iter != mButtonMap.end()) {
    iter->second->ClearState(TaskButton::State::Active);
  }

  iter = mButtonMap.find(newWindow);
  if (iter != mButtonMap.end()) {
    iter->second->ActivateState(TaskButton::State::Active);
  }

  mPane->Unlock();
}


void Taskbar::AddTask(HWND window, TaskData &taskData, bool isReplacement) {
  if (mMonitor != MONITOR_ALL && taskData.monitor != mMonitor || mButtonMap.count(window) != 0) {
    return;
  }

  mPane->Lock();

  if (isReplacement) {
    mButtons.emplace(mReplacementPosition, mPane, mButtonBackgroundPainter, mButtonTextPainter,
      mButtonEventHandler, window, taskData);
    mReplacementPosition = mButtons.end();
  } else {
    mButtons.emplace_back(mPane, mButtonBackgroundPainter, mButtonTextPainter, mButtonEventHandler,
      window, taskData);
  }
  mButtonMap[window] = --mButtons.end();

  TaskButton &button = mButtons.back();
  Relayout();
  button.Show();

  mPane->Unlock();
}


bool Taskbar::GetButtonScreenRect(HWND window, D2D1_RECT_F *rect) {
  auto iter = mButtonMap.find(window);
  if (iter == mButtonMap.end()) {
    return false;
  }
  iter->second->GetButtonScreenRect(rect);
  return true;
}


void Taskbar::Lock() {
  ++mLock;
  mPane->Lock();
}


void Taskbar::MonitorChanged(HWND window, TaskData &taskData) {
  if (taskData.monitor == mMonitor) {
    AddTask(window, taskData, false);
  } else if (mMonitor != MONITOR_ALL) {
    RemoveTask(window, false);
  }
}


void Taskbar::RemoveTask(HWND window, bool isBeingReplaced) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    // Destroying the actual button may trigger message processing, which may attempt to delete
    // the task once again (WindowMaintenance + LM_WINDOWDESTROYED). Therefore, it must be erased
    // from the mButtonMap before being erased from mButtons.
    auto buttonsIter = iter->second;
    mButtonMap.erase(iter);
    mPane->Lock();
    if (isBeingReplaced) {
      mReplacementPosition = mButtons.erase(buttonsIter);
    } else {
      mReplacementPosition = mButtons.end();
      mButtons.erase(buttonsIter);
    }
    Relayout();
    mPane->Unlock();
  }
}


void Taskbar::RedrawTask(HWND window, DWORD parts, bool flash) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    iter->second->Redraw(parts);
    if (flash) {
      iter->second->Flash();
    }
  }
}


void Taskbar::Unlock() {
  if (--mLock == 0 && mLayoutOnUnlock) {
    Relayout();
  }
  mPane->Unlock();
}


void Taskbar::UpdateButtonState(HWND window) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    iter->second->UpdateState();
  }
}
