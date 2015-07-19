#pragma once

#include "TaskButton.hpp"

#include "../nShared/LayoutSettings.hpp"

#include "../nCoreApi/IDiscardablePainter.hpp"
#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IPane.hpp"

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
  void ActiveWindowChanged(HWND oldWindow, HWND newWindow);
  void AddTask(HWND, TaskData&, bool isReplacement);
  bool GetButtonScreenRect(HWND window, D2D1_RECT_F*);
  void Lock();
  void MonitorChanged(HWND, TaskData&);
  void RedrawTask(HWND window, DWORD parts, bool flash);
  void RemoveTask(HWND window, bool isBeingReplaced);
  void Unlock();
  void UpdateButtonState(HWND window);

  void Initialized();

private:
  void Relayout();

  // Settings
private:
  LayoutSettings mLayoutSettings;
  NLENGTH mButtonMaxWidth;
  NLENGTH mButtonMaxHeight;
  NLENGTH mButtonWidth;
  NLENGTH mButtonHeight;
  UINT mMonitor;

private:
  IPane *mPane;
  IDiscardablePainter *mBackgroundPainter;
  IEventHandler *mEventHandler;
  int mLock;
  bool mLayoutOnUnlock;

private:
  IDiscardablePainter *mButtonTextPainter;
  IDiscardablePainter *mButtonBackgroundPainter;
  IEventHandler *mButtonEventHandler;
  std::list<TaskButton> mButtons;
  std::unordered_map<HWND, std::list<TaskButton>::iterator> mButtonMap;
  std::list<TaskButton>::const_iterator mReplacementPosition;

  // ButtonSettings
};
