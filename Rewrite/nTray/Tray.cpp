#include "Tray.hpp"

#include "../nCoreApi/Core.h"

extern HWND gTrayNotifyWindow;


Tray::Tray(LPCWSTR name)
  : mLock(1)
{
  ISettingsReader *reader = nCore::CreateSettingsReader(name);

  StatePainterInitData painterInitData;
  ZeroMemory(&painterInitData, sizeof(StatePainterInitData));
  painterInitData.cbSize = sizeof(StatePainterInitData);
  painterInitData.numStates = 0;
  painterInitData.states = nullptr;
  painterInitData.settingsReader = reader;
  mPainter = nCore::CreateStatePainter(&painterInitData);

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.flags = 0;
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  IPanePainter *painters[] = { mPainter, nCore::GetChildPainter() };
  paneInitData.numPainters = _countof(painters);
  paneInitData.painters = painters;
  paneInitData.settingsReader = reader;
  mPane = nCore::CreatePane(&paneInitData);

  ISettingsReader *iconReader = reader->CreateChild(L"Icon");

  StatePainterInitData buttonPainterInitData;
  ZeroMemory(&buttonPainterInitData, sizeof(StatePainterInitData));
  buttonPainterInitData.cbSize = sizeof(StatePainterInitData);
  buttonPainterInitData.numStates = 0;
  buttonPainterInitData.states = nullptr;
  buttonPainterInitData.settingsReader = iconReader;
  mIconPainter = nCore::CreateStatePainter(&buttonPainterInitData);

  iconReader->Destroy();

  reader->Destroy();
}


Tray::~Tray() {
  mPane->Lock();

  mIcons.clear();
  mIconPainter->Destroy();
  mPane->Destroy();
  mPainter->Destroy();
}


void Tray::Initialized() {
  --mLock;
  Relayout();
  mPane->Show();
}


std::list<TrayIcon>::iterator Tray::AddIcon(IconData &data) {
  mPane->Lock();

  mIcons.emplace_back(mPane, mIconPainter, data);
  Relayout();
  mIcons.back().Show();

  mPane->Unlock();
  return --mIcons.end();
}


void Tray::RemoveIcon(std::list<TrayIcon>::iterator icon) {
  mIcons.erase(icon);
  Relayout();
}


void Tray::Relayout() {
  if (mLock != 0) {
    return;
  }

  mPane->Lock();
  float width = mPane->GetRenderingSize().width;
  float iconSize = mPane->EvaluateLength(NLENGTH(0, 0, 16), true);
  float padding = mPane->EvaluateLength(NLENGTH(0, 0, 2), true);

  float x = padding, y = padding;
  for (TrayIcon &icon : mIcons) {
    icon.Position(
      NRECT(NLENGTH(x, 0, 0), NLENGTH(y, 0, 0), NLENGTH(x + iconSize, 0, 0), NLENGTH(y + iconSize, 0, 0)));
    x += iconSize + padding;
    if (x > width - iconSize - padding) {
      y += iconSize + padding;
      x = padding;
    }
  }
  mPane->Unlock();
}


bool Tray::WantsIcon(IconData &data) {
  return true;
}


LRESULT Tray::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (message) {
  case WM_MOUSEMOVE:
    if (IsWindow(gTrayNotifyWindow)) {
      D2D1_RECT_F r;
      mPane->GetScreenPosition(&r);
      MoveWindow(gTrayNotifyWindow, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
