#pragma once

#include "d2d1.h"
#include "NLength.hpp"

typedef class NRect {
public:
  NRect();
  NRect(const NLENGTH &left, const NLENGTH &top, const NLENGTH &right, const NLENGTH &bottom);

public:
  NLENGTH left;
  NLENGTH top;
  NLENGTH right;
  NLENGTH bottom;
} NRECT, *PNRECT;
typedef const NRECT *LPCNRECT;
