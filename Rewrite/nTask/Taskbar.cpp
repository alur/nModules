#include "Taskbar.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

StatePainterInitData::State sButtonStates[] = {
  { L"Hover", 0 }
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

  mButtons.emplace_back(mPane, mButtonPainter, mButtonEventHandler, window);
  mButtonMap[window] = --mButtons.end();

  TaskButton &button = mButtons.back();
  Relayout();
  button.Show();

  mPane->Unlock();
  return &button;
}


void Taskbar::RemoveTask(HWND window, bool isBeingReplaced) {
  auto iter = mButtonMap.find(window);
  if (iter != mButtonMap.end()) {
    mButtons.erase(iter->second);
    mButtonMap.erase(iter);
    Relayout();
  }
}
