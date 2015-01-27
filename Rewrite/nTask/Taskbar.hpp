#pragma once

#include "TaskButton.hpp"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class Taskbar : public IMessageHandler {
public:
  explicit Taskbar(LPCWSTR prefix);
  ~Taskbar();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

private:
  IEventHandler *mEventHandler;
  IPane *mPane;
  ISettingsReader *mSettingsReader;
  IStatePainter *mPainter;
};
