#include "Tray.hpp"

#include "../nShared/StringMap.hpp"

#include "../nCoreApi/Core.h"

extern HWND gTrayNotifyWindow;

static const StringMap sDefaults({
  { L"IconColor", L"0x00000000" }
});


Tray::Tray(LPCWSTR name)
  : mLock(1)
{
  ISettingsReader *reader = nCore::CreateSettingsReader(name, &sDefaults);
  mPainter = nCore::CreateBackgroundPainter(reader, nullptr, 0);

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.flags = 0;
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  IPainter *painters[] = { mPainter, nCore::GetChildPainter() };
  paneInitData.numPainters = _countof(painters);
  paneInitData.painters = painters;
  paneInitData.settingsReader = reader;
  mPane = nCore::CreatePane(&paneInitData);

  ISettingsReader *iconReader = reader->CreateChild(L"Icon");
  mIconPainter = nCore::CreateBackgroundPainter(iconReader, nullptr, 0);
  iconReader->Discard();

  reader->Discard();
}


Tray::~Tray() {
  mPane->Lock();

  mIcons.clear();
  mIconPainter->Discard();
  mPane->Discard();
  mPainter->Discard();
}


void Tray::Initialized() {
  --mLock;
  Relayout();
  mPane->Show();
}


std::list<TrayIcon>::iterator Tray::AddIcon(IconData &data) {
  mPane->Lock();

  mIcons.emplace_back(mPane, (IPainter*)mIconPainter, data);
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
      MoveWindow(gTrayNotifyWindow, int(r.left + 0.5f), int(r.top + 0.5f),
        int(r.right - r.left + 0.5f), int(r.bottom - r.top + 0.5f), FALSE);
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
