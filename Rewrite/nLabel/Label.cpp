#include "Label.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

const StateDefinition sStates[] = {
  { L"Hover",   0, 0 }, // 1
  { L"Pressed", 0, 0 }, // 2
};


Label::Label(LPCWSTR name) {
  ISettingsReader *reader = nCore::CreateSettingsReader(name);

  mEventHandler = nCore::CreateEventHandler(reader);
  mTextPainter = nCore::CreateTextPainter(reader, sStates, _countof(sStates));
  mBackgroundPainter = nCore::CreateBackgroundPainter(reader, sStates, _countof(sStates));

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  IPainter *painters[] = { mBackgroundPainter, mTextPainter, nCore::GetChildPainter() };
  paneInitData.painters = painters;
  paneInitData.numPainters = _countof(painters);
  paneInitData.settingsReader = reader;
  paneInitData.flags = PaneInitData::DynamicText;
  mPane = nCore::CreatePane(&paneInitData);

  wchar_t buffer[MAX_LINE_LENGTH];
  if (reader->GetString(L"Text", buffer, _countof(buffer), L"")) {
    mPane->SetText(buffer);
  }

  reader->Discard();

  mPane->Show();
}


Label::~Label() {
  mPane->Discard();
  mTextPainter->Discard();
  mBackgroundPainter->Discard();
  mEventHandler->Discard();
}


LRESULT Label::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  return mEventHandler->HandleMessage(window, message, wParam, lParam, nullptr);
}
