#include "Popup.hpp"

#include "../nCoreApi/Core.h"


Popup::Popup(LPCWSTR title, LPCWSTR prefix) {
  ISettingsReader *reader = nCore::CreateSettingsReader(prefix ? prefix : L"nPopup", nullptr);

  mBackgroundPainter = nCore::CreateBackgroundPainter(reader, nullptr, 0);
  mTitlePainter = nCore::CreateTextPainter(reader, nullptr, 0);

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(paneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.settingsReader = reader;
  paneInitData.messageHandler = this;
  paneInitData.name = nullptr;
  paneInitData.numStates = 0;
  paneInitData.states = nullptr;
  paneInitData.flags = PaneInitData::DynamicText;
  IPainter *painters[] = { mBackgroundPainter, nCore::GetChildPainter(), mTitlePainter };
  paneInitData.painters = painters;
  paneInitData.numPainters = _countof(painters);
  mPane = nCore::CreatePane(&paneInitData);

  reader->Discard();
}


Popup::~Popup() {
  mPane->Discard();
  mTitlePainter->Discard();
  mBackgroundPainter->Discard();
}


void Popup::Show() {
  mPane->Show();
}


void Popup::Show(int x, int y) {
  mPane->Show();
}


LRESULT Popup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  return DefWindowProc(window, message, wParam, lParam);
}
