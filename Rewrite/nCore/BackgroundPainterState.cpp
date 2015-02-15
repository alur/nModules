#include "BackgroundPainterState.hpp"

#include "../nUtilities/Macros.h"


BackgroundPainterState::BackgroundPainterState(const ISettingsReader *reader,
  BackgroundPainterState *base)
    : mBase(base) {
  DWORD color = (DWORD)reader->GetInt64(L"Color", 0x55C0448F);
  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;

  mSettings.cornerRadiusX = reader->GetLength(L"CornerRadiusX", NLENGTH(0, 0, 0));
  mSettings.cornerRadiusY = reader->GetLength(L"CornerRadiusY", NLENGTH(0, 0, 0));
  mSettings.outlineWidth = reader->GetLength(L"OutlineWidth", NLENGTH(0, 0, 0));
}


BackgroundPainterState::~BackgroundPainterState() {}


HRESULT BackgroundPainterState::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  ID2D1SolidColorBrush *brush;
  renderTarget->CreateSolidColorBrush(mColor, &brush);
  mBrush = brush;
  return S_OK;
}


void BackgroundPainterState::DiscardDeviceResources() {
  SAFERELEASE(mBrush);
}


void BackgroundPainterState::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    PerPaneData &data, const IPane *pane) const {
  D2D1_ROUNDED_RECT r;
  r.rect = pane->GetRenderingPosition();
  r.radiusX = data.cornerRadiusX;
  r.radiusY = data.cornerRadiusY;
  renderTarget->FillRoundedRectangle(r, mBrush);
}


void BackgroundPainterState::PositionChanged(const IPane *pane, PerPaneData &data,
    const D2D1_RECT_F &position, bool isResize, bool) {
  if (isResize) {
    data.cornerRadiusX = pane->EvaluateLength(mSettings.cornerRadiusX, true);
    data.cornerRadiusY = pane->EvaluateLength(mSettings.cornerRadiusY, false);
    data.outlineWidth = pane->EvaluateLength(mSettings.outlineWidth, true);
  }
}
