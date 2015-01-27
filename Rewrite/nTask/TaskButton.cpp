#include "TaskButton.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"


TaskButton::TaskButton(IPane *parent, IStatePainter *painter) {
  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.messageHandler = this;
  initData.painter = painter;

  mPane = parent->CreateChild(&initData);
}


TaskButton::~TaskButton() {
}


LRESULT TaskButton::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_LBUTTONUP:
    LSExecute(nullptr, L"!about", SW_SHOWDEFAULT);
    return 0;
  }
  return mEventHandler->HandleMessage(window, msg, wParam, lParam, this);
};


bool TaskButton::ProcessEvent() {
  return false;
}
