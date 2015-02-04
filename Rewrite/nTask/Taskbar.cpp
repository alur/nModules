#include "Taskbar.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

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


Taskbar::Taskbar(LPCWSTR prefix)
  : mPane(nullptr)
  , mPainter(nullptr)
  , mEventHandler(nullptr)
  , mLock(1)
{
  ISettingsReader *reader;
  nCore::CreateSettingsReader(prefix, &reader);
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
  initData.painter = (IPanePainter*)mPainter;

  mPane = nCore::CreatePane(&initData);

  ISettingsReader *buttonReader;
  reader->CreateChild(L"Button", &buttonReader);
  mButtonEventHandler = nCore::CreateEventHandler(buttonReader);

  StatePainterInitData buttonStatePainterInitData;
  ZeroMemory(&buttonStatePainterInitData, sizeof(StatePainterInitData));
  buttonStatePainterInitData.cbSize = sizeof(StatePainterInitData);
  buttonStatePainterInitData.numStates = _countof(sButtonStates);
  buttonStatePainterInitData.states = sButtonStates;
  buttonStatePainterInitData.settingsReader = buttonReader;
  mButtonPainter = nCore::CreateStatePainter(&buttonStatePainterInitData);

  buttonReader->Destroy();

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


void Taskbar::Relayout() {
  if (mLock != 0) {
    return;
  }

  mPane->Lock();

  float pos = 10;
  for (TaskButton &button : mButtons) {
    button.Position(NRECT(
      NLENGTH(0, 0, pos), NLENGTH(0, 0, 2), NLENGTH(0, 0, pos + 200), NLENGTH(0, 0, 38)));
    pos += 210;
  }

  mPane->Unlock();
}


LRESULT Taskbar::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_LBUTTONUP:
    LSExecute(nullptr, L"!about", SW_SHOWDEFAULT);
    return 0;
  }
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
