#include "StatePainter.hpp"

#include "../nUtilities/Macros.h"


EXPORT_CDECL(IStatePainter*) CreateStatePainter() {
  return new StatePainter();
}


StatePainter::StatePainter() : mBrush(nullptr) {
}


HRESULT StatePainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  ID2D1SolidColorBrush *brush;
  renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.6f, 0.2f, 0.5f), &brush);
  mBrush = brush;
  return S_OK;
}


void StatePainter::DiscardDeviceResources() {
  SAFERELEASE(mBrush);
}


void StatePainter::Paint(ID2D1RenderTarget *renderTarget, D2D1_RECT_F *area, IPane *pane) const {
  renderTarget->FillRectangle(area, mBrush);
}


bool StatePainter::UpdateDWMColor(DWORD, ID2D1RenderTarget*) {
  return false;
}


void StatePainter::UpdatePosition(D2D1_RECT_F) {
}


void StatePainter::Destroy() {
  delete this;
}
