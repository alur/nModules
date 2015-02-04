#include "TaskButton.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"


TaskButton::TaskButton(IPane *parent, IStatePainter *painter, IEventHandler *eventHandler, HWND window)
  : mEventHandler(eventHandler)
  , mWindow(window)
{
  mPainter = new ButtonPainter(painter);

  mIconPosition = NRECT(NLENGTH(0, 0, 0), NLENGTH(0, 0, 0), NLENGTH(0, 0, 15), NLENGTH(0, 0, 15));

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.messageHandler = this;
  initData.painter = mPainter;

  mPane = parent->CreateChild(&initData);

  mPainter->SetIcon(nCore::GetWindowIcon(window, 32));

  wchar_t windowText[256];
  GetWindowText(window, windowText, 256);
  mPane->SetText(windowText);
}


TaskButton::~TaskButton() {
  mPane->Destroy();
}


void TaskButton::GetButtonScreenRect(D2D1_RECT_F *rect) {
  mPane->GetScreenPosition(rect);
}


void TaskButton::Position(const NRECT &position) {
  mPane->Position(&position);
}


void TaskButton::Show() {
  mPane->Show();
}


void TaskButton::Redraw(DWORD parts) {
  mPane->Lock();
  if (CHECKFLAG(parts, Part::Icon)) {
    mPainter->SetIcon(nCore::GetWindowIcon(mWindow, 32));
    mPane->Repaint(&mIconPosition);
  }
  if (CHECKFLAG(parts, Part::Text)) {
    wchar_t windowText[256];
    GetWindowText(mWindow, windowText, 256);
    mPane->SetText(windowText);
  }
  mPane->Unlock();
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
