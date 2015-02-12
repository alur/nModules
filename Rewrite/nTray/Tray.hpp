#pragma once

#include "IconData.h"
#include "TrayIcon.hpp"

#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IStatePainter.hpp"

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

private:
  void Relayout();

private:
  LayoutSettings mLayoutSettings;

  IPane *mPane;
  IStatePainter *mPainter;

private:
  IStatePainter *mIconPainter;
  std::list<TrayIcon> mIcons;
};
