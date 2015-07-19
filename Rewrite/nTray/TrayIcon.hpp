#pragma once

#include "IconData.h"

#include "../nCoreApi/IImagePainter.hpp"
#include "../nCoreApi/IPane.hpp"

#include "../Headers/lsapi.h"

class TrayIcon : public IMessageHandler {
public:
  TrayIcon(IPane *parent, IPainter *painter, IconData &data);
  ~TrayIcon();

public:
  TrayIcon(TrayIcon&) = delete;
  TrayIcon &operator=(TrayIcon&) = delete;

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

public:
  bool GetScreenRect(LPRECT rect);
  void Modify(LPLSNOTIFYICONDATA);
  void Position(const NRECT &position);
  void SendCallback(UINT message, WPARAM wParam, LPARAM lParam);
  void Show();

private:
  IPane *mPane;
  IconData &mData;

  IImagePainter *mIconPainter;
};
