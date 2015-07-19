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

  mPane->SetText(title);

  reader->GetBool(L"ConfineToMonitor", false);
  reader->GetBool(L"ConfineToWorkArea", false);

  reader->Discard();
}


Popup::~Popup() {
  for (PopupItem *item : mItems) {
    delete item;
  }
  mPane->Discard();
  mTitlePainter->Discard();
  mBackgroundPainter->Discard();
}


void Popup::Show() {
  POINT pt;
  GetCursorPos(&pt);
  Show(pt.x, pt.y);
}


void Popup::Show(int x, int y) {
  mPane->Move(NPOINT(NLENGTH(x, 0, 0), NLENGTH(y, 0, 0)));
  mPane->Show();

  for (PopupItem *item : mItems) {

  }

  SetWindowPos(mPane->GetWindow(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetFocus(mPane->GetWindow());
  SetActiveWindow(mPane->GetWindow());
}


LRESULT Popup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch(message) {
    case WM_ACTIVATE:
      if (LOWORD(wParam) == WA_INACTIVE) {
        mPane->Hide();
      }
      return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
