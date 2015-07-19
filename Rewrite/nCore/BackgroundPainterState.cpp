#include "BackgroundPainterState.hpp"

#include "../Headers/Macros.h"


void GetLength(const ISettingsReader *reader, LPCWSTR key, FallbackOptional<NLENGTH> &out) {
  NLENGTH value;
  if (reader->GetLength(key, &value)) {
    out = value;
  }
}


void GetColor(const ISettingsReader *reader, LPCWSTR key, FallbackOptional<D2D1_COLOR_F> &out) {
  INT64 value;
  if (reader->GetInt64(key, &value)) {
    out = D2D1::ColorF((value >> 16 & 0xFF) / 255.0f, (value >> 8 & 0xFF) / 255.0f,
      (value & 0xFF) / 255.0f, (value >> 24) / 255.0f);
  }
}


BackgroundPainterState::BackgroundPainterState(const ISettingsReader *reader,
  BackgroundPainterState *base)
    : mBase(base)
    , mCornerRadiusX(base->mCornerRadiusX)
    , mCornerRadiusY(base->mCornerRadiusY)
    , mOutlineWidth(base->mOutlineWidth)
    , mColor(base->mColor)
{
  if (mBase) {
    mBase->mDependents.push_back(this);
  } else {
    mCornerRadiusX = NLENGTH(0, 0, 0);
    mCornerRadiusY = NLENGTH(0, 0, 0);
    mOutlineWidth = NLENGTH(0, 0, 0);
    mColor = D2D1::ColorF(0, 0, 0, 0.5f);
  }

  GetColor(reader, L"Color", mColor);
  GetLength(reader, L"CornerRadiusX", mCornerRadiusX);
  GetLength(reader, L"CornerRadiusY", mCornerRadiusY);
  GetLength(reader, L"OutlineWidth", mOutlineWidth);
}


BackgroundPainterState::~BackgroundPainterState() {}


HRESULT BackgroundPainterState::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  ID2D1SolidColorBrush *brush;
  renderTarget->CreateSolidColorBrush(mColor.Get(), &brush);
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
    data.cornerRadiusX = pane->EvaluateLength(mCornerRadiusX.Get(), true);
    data.cornerRadiusY = pane->EvaluateLength(mCornerRadiusY.Get(), false);
    data.outlineWidth = pane->EvaluateLength(mOutlineWidth.Get(), true);
  }
}
