#pragma once

#include "ButtonPainter.hpp"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

#include <thread>

class TaskButton : public IMessageHandler, public IEventProcessor {
public:
  enum Part {
    Text = 1,
    Icon = 2,
    OverlayIcon = 4,
    Progress = 8
  };

  enum class State : BYTE {
    Minimized = 1,
    Flashing = 2,
    MinimizedFlashing = 3,
    Active = 4,
    Hover = 5,
    MinimizedHober = 6,
    ActiveHover = 7,
    FlashingHover = 8,
    MinimizedFlashingHover = 9
  };

  BYTE operator()(State &state) {
    return (BYTE)state;
  }

public:
  TaskButton(IPane *pane, IStatePainter *painter, IEventHandler *eventHandler, HWND window);
  ~TaskButton();

public:
  TaskButton(const TaskButton&) = delete;
  TaskButton &operator=(const TaskButton&) = delete;

public:
  void GetButtonScreenRect(D2D1_RECT_F *rect);
  void Position(const NRECT &position);
  void Redraw(DWORD sections);
  void Show();
  void MoveMouseToWindow();

  // Button events
private:
  void SelectTask();
  void OpenTaskProcess();
  void ShowContextMenu();
  void ToggleAlwaysOnTop();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

  // IEventProcessor
public:
  bool APICALL ProcessEvent(LPCWSTR action) override;

private:
  const HWND mWindow;
  IEventHandler *mEventHandler;
  ButtonPainter *mPainter;
  IPane *mPane;

  HMENU mMenu;
  std::thread mMenuThread;
  HWND mMenuWindow;

  // Things that should be under ButtonSettings.
private:
  NRECT mIconPosition;
};
