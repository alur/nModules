#pragma once

#include "PopupItem.hpp"

#include "../nCoreApi/IDiscardablePainter.hpp"
#include "../nCoreApi/IPane.hpp"

#include "../Headers/Windows.h"

#include <vector>

class Popup : public IMessageHandler {
public:
  Popup(LPCWSTR title, LPCWSTR prefix);
  virtual ~Popup();

public:
  void AddItem(PopupItem *item);
  void Show();
  void Show(int x, int y);

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

public:
  IPane *mPane;
  IDiscardablePainter *mTitlePainter;
  IDiscardablePainter *mBackgroundPainter;

  std::vector<PopupItem*> mItems;
};
