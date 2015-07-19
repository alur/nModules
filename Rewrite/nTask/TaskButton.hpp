#pragma once

#include "TaskData.h"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/IImagePainter.hpp"
#include "../nCoreApi/IPane.hpp"

#include <thread>

class TaskButton : public IMessageHandler, public IEventProcessor {
public:
  enum Part {
    Text = 1,
    Icon = 2,
    OverlayIcon = 4,
    Progress = 8
  };

  enum State : BYTE {
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

public:
  TaskButton(IPane *pane, IPainter *bcakgroundPainter, IPainter *textPainter,
    IEventHandler *eventHandler, HWND window, TaskData &taskData);
  ~TaskButton();

public:
  TaskButton(const TaskButton&) = delete;
  TaskButton &operator=(const TaskButton&) = delete;

public:
  void ActivateState(State state);
  void ClearState(State state);
  void Flash();
  void GetButtonScreenRect(D2D1_RECT_F *rect);
  void MoveMouseToWindow();
  void Position(const NRECT &position);
  void Redraw(DWORD sections);
  void Show();
  void UpdateState();

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
  TaskData &mTaskData;
  IEventHandler * const mEventHandler;
  IImagePainter *mIconPainter;
  IImagePainter *mOverlayIconPainter;
  IPane *mPane;

  HMENU mMenu;
  std::thread mMenuThread;
  HWND mMenuWindow;

  UINT_PTR mFlashInterval;

  // Things that should be under ButtonSettings.
private:
  NRECT mIconPosition;
  NRECT mOverlayIconPosition;
};
