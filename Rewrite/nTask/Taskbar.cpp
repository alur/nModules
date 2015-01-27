#include "Taskbar.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"


Taskbar::Taskbar(LPCWSTR prefix)
  : mPane(nullptr)
  , mPainter(nullptr)
  , mEventHandler(nullptr)
  , mSettingsReader(nullptr)
{
  nCore::CreateSettingsReader(prefix, &mSettingsReader);
  mEventHandler = nCore::CreateEventHandler(mSettingsReader);

  StatePainterInitData spid;
  spid.cbSize = sizeof(StatePainterInitData);
  spid.numStates = 0;
  spid.inheritance = {};
  spid.stateNames = {};

  mPainter = nCore::CreateStatePainter(&spid, mSettingsReader);

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.name = prefix;
  initData.settingsReader = mSettingsReader;
  initData.messageHandler = (IMessageHandler*)this;
  initData.painter = (IPanePainter*)mPainter;

  mPane = nCore::CreatePane(&initData);
}


Taskbar::~Taskbar() {
  mPane->Destroy();
  mPainter->Destroy();
  mSettingsReader->Destroy();
  mEventHandler->Destroy();
}


LRESULT Taskbar::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_LBUTTONUP:
    LSExecute(nullptr, L"!about", SW_SHOWDEFAULT);
    return 0;
  }
  return mEventHandler->HandleMessage(window, msg, wParam, lParam, nullptr);
};
