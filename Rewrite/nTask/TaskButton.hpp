#pragma once

#include "ButtonPainter.hpp"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class TaskButton : public IMessageHandler, public IEventProcessor {
public:
  enum Part {
    Text = 1,
    Icon = 2,
    OverlayIcon = 4,
    Progress = 8
  };

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

  // Button events
private:
  //
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
  HMENU mMenu;
  IEventHandler *mEventHandler;
  ButtonPainter *mPainter;
  IPane *mPane;

  // Things that should be under ButtonSettings.
private:
  NRECT mIconPosition;
};
