#pragma once

#include "../Headers/d2d1.h"

class PopupItem {
public:
  PopupItem();
  virtual ~PopupItem();

public:
  virtual D2D1_SIZE_F GetDesiredSize(D2D1_SIZE_F maxSize) const = 0;
};
