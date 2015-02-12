#pragma once

#include "../nCoreApi/IPane.hpp"

class TrayIcon {
public:
  TrayIcon(IPane *parent);
  ~TrayIcon();

public:
  bool GetScreenRect(LPRECT rect);

private:
  IPane *mPane;
};
