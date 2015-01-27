#pragma once

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class TaskButton : public IMessageHandler, public IEventProcessor {
public:
  TaskButton(IPane *pane, IStatePainter *painter);
  ~TaskButton();

public:
  void Position(NRECT position);

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

  // IEventProcessor
public:
  bool APICALL ProcessEvent() override;

private:
  IEventHandler *mEventHandler;
  IPane *mPane;
  ISettingsReader *mSettingsReader;
};
