#include "Tray.hpp"

#include "../nCoreApi/Core.h"

Tray::Tray(LPCWSTR name) {
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
  paneInitData.numPainters = 1;
  IPanePainter *painter = mPainter;
  paneInitData.painters = &painter;
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
  mPane->Show();
}


Tray::~Tray() {
  mPane->Lock();

  mIcons.clear();
  mIconPainter->Destroy();
  mPane->Destroy();
  mPainter->Destroy();
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
  return DefWindowProc(window, message, wParam, lParam);
}
