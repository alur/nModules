#pragma once

#include "IPanePainter.hpp"

class IImagePainter : public IPanePainter {
public:
  virtual void APICALL Destroy() = 0;
  virtual void APICALL SetPosition(NRECT position, const IPane *pane) = 0;
  virtual void APICALL SetImage(HICON icon) = 0;
  virtual void APICALL SetImage(HBITMAP bitmap) = 0;
};
