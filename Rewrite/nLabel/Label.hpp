#pragma once

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class Label : public IMessageHandler {
public:
  explicit Label(LPCWSTR name);
  ~Label();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

private:
  IPane *mPane;
  IStatePainter *mPainter;
  IEventHandler *mEventHandler;
};
