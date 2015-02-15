#pragma once

#include "IconData.h"
#include "TrayIcon.hpp"

#include "../nCoreApi/IPane.hpp"

#include "../nShared/LayoutSettings.hpp"

#include <list>

class Tray : public IMessageHandler {
public:
  explicit Tray(LPCWSTR name);
  ~Tray();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

public:
  std::list<TrayIcon>::iterator AddIcon(IconData&);
  void RemoveIcon(std::list<TrayIcon>::iterator);
  bool WantsIcon(IconData&);
  void Initialized();

private:
  void Relayout();

private:
  LayoutSettings mLayoutSettings;

  IPane *mPane;
  IDiscardablePainter *mPainter;
  int mLock;

private:
  IDiscardablePainter *mIconPainter;
  std::list<TrayIcon> mIcons;
};
