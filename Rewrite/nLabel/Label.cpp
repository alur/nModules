#include "Label.hpp"

#include "../nCoreApi/Core.h"


Label::Label(LPCWSTR name) {
  ISettingsReader *settingsReader;
  nCore::CreateSettingsReader(name, &settingsReader);

  mEventHandler = nCore::CreateEventHandler(settingsReader);

  StatePainterInitData painterInitData;
  ZeroMemory(&painterInitData, sizeof(StatePainterInitData));
  painterInitData.cbSize = sizeof(StatePainterInitData);
  painterInitData.numStates = 0;
  painterInitData.settingsReader = settingsReader;
  painterInitData.states = nullptr;
  mPainter = nCore::CreateStatePainter(&painterInitData);

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  paneInitData.painter = mPainter;
  paneInitData.settingsReader = settingsReader;
  mPane = nCore::CreatePane(&paneInitData);

  settingsReader->Destroy();

  mPane->Show();
}


Label::~Label() {
  mPane->Destroy();
  mPainter->Destroy();
  mEventHandler->Destroy();
}


LRESULT Label::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  return mEventHandler->HandleMessage(window, message, wParam, lParam, nullptr);
}
