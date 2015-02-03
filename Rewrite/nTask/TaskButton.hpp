#pragma once

#include "ButtonPainter.hpp"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class TaskButton : public IMessageHandler, public IEventProcessor {
public:
  TaskButton(IPane *pane, IStatePainter *painter, IEventHandler *eventHandler, HWND window);
  ~TaskButton();

public:
  TaskButton(const TaskButton&) = delete;
  TaskButton &operator=(const TaskButton&) = delete;

public:
  void Position(const NRECT &position);
  void Redraw();
  void Show();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

  // IEventProcessor
public:
  bool APICALL ProcessEvent() override;

private:
  const HWND mWindow;
  IEventHandler *mEventHandler;
  ButtonPainter *mPainter;
  IPane *mPane;
};
