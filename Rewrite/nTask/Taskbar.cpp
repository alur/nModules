#include "Taskbar.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

#include <assert.h>
#include <algorithm>

// TODO(Erik):Simplify this
StatePainterInitData::State sButtonStates[] = {
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

/*DefaultSettings sButtonDefaults = {

};*/

Taskbar::Taskbar(LPCWSTR prefix)
  : mPane(nullptr)
  , mPainter(nullptr)
  , mEventHandler(nullptr)
  , mLock(1)
{
  mReplacementPosition = mButtons.end();

  ISettingsReader *reader = nCore::CreateSettingsReader(prefix);
  mEventHandler = nCore::CreateEventHandler(reader);

  StatePainterInitData spid;
  spid.cbSize = sizeof(StatePainterInitData);
  spid.numStates = 0;
  spid.settingsReader = reader;
  mPainter = nCore::CreateStatePainter(&spid);

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.name = prefix;
  initData.settingsReader = reader;
  initData.messageHandler = (IMessageHandler*)this;
  IPanePainter *painter = (IPanePainter*)mPainter;
  initData.painters = &painter;
  initData.numPainters = 1;

  mPane = nCore::CreatePane(&initData);

  ISettingsReader *buttonReader = reader->CreateChild(L"Button");
  mButtonEventHandler = nCore::CreateEventHandler(buttonReader);

  StatePainterInitData buttonStatePainterInitData;
  ZeroMemory(&buttonStatePainterInitData, sizeof(StatePainterInitData));
  buttonStatePainterInitData.cbSize = sizeof(StatePainterInitData);
  buttonStatePainterInitData.numStates = _countof(sButtonStates);
  buttonStatePainterInitData.states = sButtonStates;
  buttonStatePainterInitData.settingsReader = buttonReader;
  mButtonPainter = nCore::CreateStatePainter(&buttonStatePainterInitData);

  mButtonWidth = buttonReader->GetLength(L"Width", NLENGTH(0, 0, 150));
  mButtonHeight = buttonReader->GetLength(L"Height", NLENGTH(0, 0, 36));
  mButtonMaxWidth = buttonReader->GetLength(L"MaxWidth", mButtonWidth);
  mButtonMaxHeight = buttonReader->GetLength(L"MaxHeight", mButtonHeight);

  buttonReader->Destroy();

  mLayoutSettings.Load(reader);

  reader->Destroy();
}


Taskbar::~Taskbar() {
  mPane->Lock(); // Never paint again.

  mButtons.clear();
  mButtonPainter->Destroy();
  mButtonEventHandler->Destroy();

  mPane->Destroy();
  mPainter->Destroy();
  mEventHandler->Destroy();
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
    return;
  }

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


TaskButton *Taskbar::AddTask(HWND window, bool isReplacement) {
  mPane->Lock();

  if (isReplacement) {
    mButtons.emplace(mReplacementPosition, mPane, mButtonPainter, mButtonEventHandler, window);
  } else {
    mButtons.emplace_back(mPane, mButtonPainter, mButtonEventHandler, window);
  }
  mButtonMap[window] = --mButtons.end();

  TaskButton &button = mButtons.back();
  Relayout();
  button.Show();

  mPane->Unlock();
  return &button;
}


bool Taskbar::GetButtonScreenRect(HWND window, D2D1_RECT_F *rect) {
  auto iter = mButtonMap.find(window);
  if (iter == mButtonMap.end()) {
    return false;
  }
  iter->second->GetButtonScreenRect(rect);
  return true;
}


void Taskbar::RemoveTask(HWND window, bool isBeingReplaced) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    mPane->Lock();
    if (isBeingReplaced) {
      mReplacementPosition = mButtons.erase(iter->second);
    } else {
      mButtons.erase(iter->second);
    }
    mButtonMap.erase(iter);
    Relayout();
    mPane->Unlock();
  }
}


void Taskbar::RedrawTask(HWND window, DWORD parts) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    iter->second->Redraw(parts);
  }
}


void Taskbar::SetOverlayIcon(HWND window, HICON icon) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    iter->second->SetOverlayIcon(icon);
  }
}
