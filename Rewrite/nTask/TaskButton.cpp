#include "TaskButton.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"


TaskButton::TaskButton(IPane *parent, IStatePainter *painter, IEventHandler *eventHandler, HWND window)
  : mEventHandler(eventHandler)
  , mWindow(window)
{
  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.messageHandler = this;
  initData.painter = painter;

  mPane = parent->CreateChild(&initData);

  wchar_t windowText[256];
  GetWindowText(window, windowText, 256);
  mPane->SetText(windowText);
}


TaskButton::~TaskButton() {
  mPane->Destroy();
}


void TaskButton::Position(const NRECT &position) {
  mPane->Position(&position);
}


void TaskButton::Show() {
  mPane->Show();
}


LRESULT TaskButton::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_LBUTTONUP:
    if (IsIconic(mWindow)) {
      BringWindowToTop(mWindow);
      PostMessage(mWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
      SetForegroundWindow(mWindow);
      /*if (gActiveWindowTracking != FALSE) {
        MoveMouseToWindow();
      }*/
    } else if (GetForegroundWindow() == mWindow) {
      PostMessage(mWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      //ActivateState(State::Minimized);
    } else {
      SetForegroundWindow(mWindow);
      /*if (gActiveWindowTracking != FALSE) {
        MoveMouseToWindow();
      }*/
    }
    //((Taskbar*)mParent)->HideThumbnail();
    return 0;
  }
  return mEventHandler->HandleMessage(window, msg, wParam, lParam, this);
};


bool TaskButton::ProcessEvent() {
  return false;
}
