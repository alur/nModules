#pragma once

#include "TaskButton.hpp"

#include "../nShared/LayoutSettings.hpp"

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
  bool GetButtonScreenRect(HWND window, D2D1_RECT_F*);
  void RedrawTask(HWND window, DWORD parts);
  void RemoveTask(HWND window, bool isBeingReplaced);
  void SetOverlayIcon(HWND window, HICON icon);

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
