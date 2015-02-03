#pragma once

#include "TaskButton.hpp"

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

#include <list>
#include <unordered_map>

class Taskbar : public IMessageHandler {
public:
  explicit Taskbar(LPCWSTR prefix);
  ~Taskbar();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

public:
  TaskButton *AddTask(HWND, bool isReplacement);
  void RedrawTask(HWND window);
  void RemoveTask(HWND window, bool isBeingReplaced);

  void Initialized();

private:
  void Relayout();

private:
  IPane *mPane;
  IStatePainter *mPainter;
  IEventHandler *mEventHandler;
  int mLock;

private:
  IStatePainter *mButtonPainter;
  IEventHandler *mButtonEventHandler;
  std::list<TaskButton> mButtons;
  std::unordered_map<HWND, std::list<TaskButton>::iterator> mButtonMap;
  std::list<TaskButton>::const_iterator mReplacementPosition;

  // ButtonSettings
};
