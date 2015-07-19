#include "TaskButton.hpp"

#include "../nCoreApi/Core.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <Shellapi.h>

extern BOOL gActiveWindowTracking;
extern HWND gActiveWindow;
extern HWND gPreviouslyActiveWindow;
extern StateDefinition gButtonStates[];
extern BYTE gNumButtonStates;


TaskButton::TaskButton(IPane *parent, IPainter *backgroundPainter, IPainter *textPainter,
    IEventHandler *eventHandler, HWND window, TaskData &taskData)
  : mEventHandler(eventHandler)
  , mIconPainter(nullptr)
  , mMenu(nullptr)
  , mMenuWindow(nullptr)
  , mOverlayIconPainter(nullptr)
  , mTaskData(taskData)
  , mFlashInterval(0)
  , mWindow(window)
{
  mIconPosition = NRECT(NLENGTH(0, 0, 0), NLENGTH(0, 0, 0), NLENGTH(0, 0, 32), NLENGTH(0, 0, 32));
  mIconPainter = nCore::CreateImagePainter();
  mIconPainter->SetPosition(mIconPosition, nullptr);
  mIconPainter->SetImage(nCore::GetWindowIcon(window, 32));

  mOverlayIconPosition =
    NRECT(NLENGTH(0, 0, 16), NLENGTH(0, 0, 16), NLENGTH(0, 0, 32), NLENGTH(0, 0, 32));
  mOverlayIconPainter = nCore::CreateImagePainter();
  mOverlayIconPainter->SetPosition(mOverlayIconPosition, nullptr);

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.messageHandler = this;
  IPainter *painters[] = { backgroundPainter, textPainter, mIconPainter, mOverlayIconPainter };
  initData.painters = painters;
  initData.numPainters = _countof(painters);
  initData.states = gButtonStates;
  initData.numStates = gNumButtonStates;
  mPane = parent->CreateChild(&initData);

  if (taskData.minimized) {
    ActivateState(State::Minimized);
  }
  if (window == gActiveWindow) {
    ActivateState(State::Active);
  }

  wchar_t windowText[256];
  GetWindowText(window, windowText, 256);
  mPane->SetText(windowText);

  // Reset the system menu for the window
  mMenu = GetSystemMenu(mWindow, FALSE);
  if (!IsMenu(mMenu)) {
    GetSystemMenu(mWindow, TRUE);
    mMenu = GetSystemMenu(mWindow, FALSE);
  }
}


TaskButton::~TaskButton() {
  if (mMenuThread.joinable()) {
    PostMessage(mMenuWindow, WM_CANCELMODE, 0, 0);
    mMenuThread.join();
  }
  if (mFlashInterval) {
    nCore::ClearInterval(mFlashInterval);
  }
  mPane->Discard();
  mIconPainter->Discard();
  mOverlayIconPainter->Discard();
}


void TaskButton::ActivateState(State state) {
  mPane->ActivateState(state);
  if (state == State::Active) {
    if (mFlashInterval) {
      nCore::ClearInterval(mFlashInterval);
      mFlashInterval = 0;
      ClearState(State::Flashing);
    }
    ClearState(State::Minimized);
  } else if (state == State::Minimized) {
    mTaskData.minimized = true;
  }
}


void TaskButton::ClearState(State state) {
  mPane->ClearState(state);
  if (state == State::Minimized) {
    mTaskData.minimized = false;
  }
}


void TaskButton::Flash() {
  if (!mFlashInterval) {
    ActivateState(State::Flashing);
    mFlashInterval = nCore::SetInterval(500, this);
  }
}


void TaskButton::GetButtonScreenRect(D2D1_RECT_F *rect) {
  WINDOWPLACEMENT wp;
  GetWindowPlacement(mWindow, &wp);
  if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) {
    // The window is being minimized.
    ActivateState(State::Minimized);
  }
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
  if (CHECKFLAG(parts, Part::Text)) {
    wchar_t windowText[256];
    GetWindowText(mWindow, windowText, 256);
    mPane->SetText(windowText);
  }
  if (CHECKFLAG(parts, Part::Icon)) {
    mIconPainter->SetImage(nCore::GetWindowIcon(mWindow, 32));
    mPane->Repaint(&mIconPosition);
  }
  if (CHECKFLAG(parts, Part::OverlayIcon)) {
    mOverlayIconPainter->SetImage(mTaskData.overlayIcon);
    mPane->Repaint(&mOverlayIconPosition);
  }
  mPane->Unlock();
}


LRESULT TaskButton::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_MOUSEMOVE:
    ActivateState(State::Hover);
    return 0;

  case WM_MOUSELEAVE:
    ClearState(State::Hover);
    return 0;

  case WM_TIMER:
    if (wParam == mFlashInterval) {
      mPane->ToggleState(State::Flashing);
    }
    return 0;

  case WM_LBUTTONUP:
    SelectTask();
    return 0;

  case WM_RBUTTONUP:
    ShowContextMenu();
    return 0;

  case WM_MBUTTONUP:
    OpenTaskProcess();
    return 0;
  }
  return mEventHandler->HandleMessage(window, msg, wParam, lParam, this);
};


bool TaskButton::ProcessEvent(LPCWSTR) {
  return false;
}


void TaskButton::SelectTask() {
  if (IsIconic(mWindow)) {
    if (gActiveWindowTracking != FALSE) {
      MoveMouseToWindow();
    }
    BringWindowToTop(mWindow);
    PostMessage(mWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(mWindow);
  } else if (gPreviouslyActiveWindow == mWindow) {
    PostMessage(mWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    ActivateState(State::Minimized);
  } else {
    if (gActiveWindowTracking != FALSE) {
      MoveMouseToWindow();
    }
    SetForegroundWindow(mWindow);
  }
  //((Taskbar*)mParent)->HideThumbnail();
}


void TaskButton::MoveMouseToWindow() {
  WINDOWPLACEMENT placement;
  RECT &rect = placement.rcNormalPosition;
  GetWindowPlacement(mWindow, &placement);
  SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
}


void TaskButton::OpenTaskProcess() {
  DWORD processId;
  GetWindowThreadProcessId(mWindow, &processId);
  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
  if (process) {
    wchar_t processPath[MAX_PATH];
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageName(process, 0, processPath, &size)) {
      ShellExecute(nullptr, L"open", processPath, L"", L"", SW_SHOW);
    }
    CloseHandle(process);
  }
}


void TaskButton::ShowContextMenu() {
  if (mMenuThread.joinable()) {
    PostMessage(mMenuWindow, WM_CANCELMODE, 0, 0);
    mMenuThread.join();
  }

  mMenuThread = std::thread([this] () {
    SetWindowPos(mWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    WINDOWPLACEMENT wp;

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(mWindow, &wp);

    // Select which menu items are enabled
    EnableMenuItem(mMenu, SC_RESTORE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_SIZE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MINIMIZE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // Let the application modify the menu
    PostMessage(mWindow, WM_INITMENU, (WPARAM)mMenu, 0);
    PostMessage(mWindow, WM_INITMENUPOPUP, (WPARAM)mMenu, MAKELPARAM(0, TRUE));

    POINT pt;
    GetCursorPos(&pt);
    mMenuWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"Static", L"", WS_POPUP, pt.x,
      pt.y, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow(mMenuWindow, SW_SHOW);

    int command = TrackPopupMenu(mMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0,
      mMenuWindow, NULL);
    if (command != 0) {
      PostMessage(mWindow, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
    DestroyWindow(mMenuWindow);
    mMenuWindow = nullptr;
  });
}


void TaskButton::ToggleAlwaysOnTop() {
  if (CHECKFLAG(GetWindowLongPtr(mWindow, GWL_EXSTYLE), WS_EX_TOPMOST)) {
    SetWindowPos(mWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  } else {
    SetWindowPos(mWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
}


void TaskButton::UpdateState() {
  if (mTaskData.minimized) {
    ActivateState(State::Minimized);
  } else {
    ClearState(State::Minimized);
  }
}
