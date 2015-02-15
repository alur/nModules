#pragma once

#include "../nCoreApi/IDiscardablePainter.hpp"
#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IPane.hpp"

class Label : public IMessageHandler {
public:
  explicit Label(LPCWSTR name);
  ~Label();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

private:
  IPane *mPane;
  IDiscardablePainter *mTextPainter;
  IDiscardablePainter *mBackgroundPainter;
  IEventHandler *mEventHandler;
};
