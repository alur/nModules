#pragma once

#include "IDiscardablePainter.hpp"

class IImagePainter : public IDiscardablePainter {
public:
  virtual void APICALL SetPosition(NRECT position, const IPane *pane) = 0;
  virtual void APICALL SetImage(HICON icon) = 0;
  virtual void APICALL SetImage(HBITMAP bitmap) = 0;
};
