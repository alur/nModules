#include "ButtonPainter.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/Macros.h"

#include <Wincodec.h>


ButtonPainter::ButtonPainter(IStatePainter *statePainter, const NRECT &iconPosition)
  : mStatePainter(statePainter)
  , mIconPosition(iconPosition)
  , mIconBrush(nullptr)
  , mIcon(nullptr)
  , mRenderTarget(nullptr)
{
}


ButtonPainter::~ButtonPainter() {
}


LPVOID ButtonPainter::AddPane(const IPane *pane) {
  return mStatePainter->AddPane(pane);
};


HRESULT ButtonPainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  mRenderTarget = renderTarget;
  HRESULT hr = mStatePainter->CreateDeviceResources(renderTarget);
  if (SUCCEEDED(hr) && mIcon) {
    if (SUCCEEDED(BrushFromIcon(mIcon, renderTarget, &mIconSize, &mIconBrush))) {
      mIconBrush->SetTransform(GetTransform());
    }
  }
  return hr;
}


void ButtonPainter::DiscardDeviceResources() {
  mStatePainter->DiscardDeviceResources();
  SAFERELEASE(mIconBrush);
  mRenderTarget = nullptr;
}


bool ButtonPainter::DynamicColorChanged(ID2D1RenderTarget *renderTarget) {
  return mStatePainter->DynamicColorChanged(renderTarget);
}


void ButtonPainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const {
  mStatePainter->Paint(renderTarget, area, pane, painterData);
  if (mIconBrush) {
    mRenderTarget->FillRectangle(mIconPaintingPosition, mIconBrush);
  }
}


void ButtonPainter::PositionChanged(const IPane *pane, LPVOID painterData,
    const D2D1_RECT_F &position, bool isMove, bool isSize) {
  mStatePainter->PositionChanged(pane, painterData, position, isMove, isSize);
  mIconPaintingPosition = D2D1::RectF(
    pane->EvaluateLength(mIconPosition.left, true) + position.left,
    pane->EvaluateLength(mIconPosition.top, true) + position.top,
    pane->EvaluateLength(mIconPosition.right, true) + position.left,
    pane->EvaluateLength(mIconPosition.bottom, true) + position.top);
  if (mIconBrush) {
    mIconBrush->SetTransform(GetTransform());
  }
}


void ButtonPainter::RemovePane(const IPane *pane, LPVOID painterData) {
  mStatePainter->RemovePane(pane, painterData);
}


void ButtonPainter::SetIcon(HICON icon) {
  mIcon = icon;
  if (mRenderTarget && mIcon) {
    SAFERELEASE(mIconBrush);
    if (SUCCEEDED(BrushFromIcon(mIcon, mRenderTarget, &mIconSize, &mIconBrush))) {
      mIconBrush->SetTransform(GetTransform());
    }
  }
}


void ButtonPainter::TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) {
  mStatePainter->TextChanged(pane, painterData, text);
}


HRESULT ButtonPainter::BrushFromIcon(HICON icon, ID2D1RenderTarget *renderTarget,
    D2D1_SIZE_U *size, ID2D1BitmapBrush **brush) {
  IWICImagingFactory *factory = nCore::GetWICFactory();
  IWICBitmap *wicBitmap;
  HRESULT hr = factory->CreateBitmapFromHICON(mIcon, &wicBitmap);
  if (SUCCEEDED(hr)) {
    IWICFormatConverter *converter;
    hr = factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(hr)) {
      hr = converter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
        nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
      if (SUCCEEDED(hr)) {
        ID2D1Bitmap *bitmap;
        hr = renderTarget->CreateBitmapFromWicBitmap(converter, &bitmap);
        if (SUCCEEDED(hr)) {
          mIconSize = bitmap->GetPixelSize();
          hr = renderTarget->CreateBitmapBrush(bitmap, brush);
          bitmap->Release();
        }
      }
      converter->Release();
    }
    wicBitmap->Release();
  }
  return hr;
}


D2D1_MATRIX_3X2_F ButtonPainter::GetTransform() const {
  return D2D1::Matrix3x2F::Scale(
    (mIconPaintingPosition.right - mIconPaintingPosition.left) / (float)mIconSize.width,
    (mIconPaintingPosition.bottom - mIconPaintingPosition.top) / (float)mIconSize.height)
    * D2D1::Matrix3x2F::Translation(mIconPaintingPosition.left, mIconPaintingPosition.top);
}
