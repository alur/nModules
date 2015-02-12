#include "Label.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

StatePainterInitData::State sStates[] = {
  { L"Hover",   0, 0 }, // 1
  { L"Pressed", 0, 0 }, // 2
};

Label::Label(LPCWSTR name) {
  ISettingsReader *settingsReader;
  nCore::CreateSettingsReader(name, &settingsReader);

  mEventHandler = nCore::CreateEventHandler(settingsReader);

  StatePainterInitData painterInitData;
  ZeroMemory(&painterInitData, sizeof(StatePainterInitData));
  painterInitData.cbSize = sizeof(StatePainterInitData);
  painterInitData.numStates = 2;
  painterInitData.settingsReader = settingsReader;
  painterInitData.states = sStates;
  mPainter = nCore::CreateStatePainter(&painterInitData);

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  IPanePainter *painter = (IPanePainter*)mPainter;
  paneInitData.painters = &painter;
  paneInitData.numPainters = 1;
  paneInitData.settingsReader = settingsReader;
  paneInitData.flags = PaneInitData::DynamicText;
  mPane = nCore::CreatePane(&paneInitData);

  wchar_t buffer[MAX_LINE_LENGTH];
  if (settingsReader->GetString(L"Text", buffer, _countof(buffer), L"")) {
    mPane->SetText(buffer);
  }

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
