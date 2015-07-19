#include "Api.h"
#include "ImagePainter.hpp"

#include "../nShared/Math.h"

#include "../Headers/Macros.h"

#include <assert.h>
#include <Wincodec.h>


EXPORT_CDECL(IImagePainter*) CreateImagePainter() {
  return new ImagePainter();
}


ImagePainter::ImagePainter()
  : mBrush(nullptr)
  , mImage(nullptr)
  , mRenderTarget(nullptr)
  , mPosition(NLENGTH(0, 0, 0), NLENGTH(0, 0, 0), NLENGTH(0, 1, 0), NLENGTH(0, 1, 0))
{
}


ImagePainter::~ImagePainter() {
  assert(!mBrush); // Should have discarded already.
  SAFERELEASE(mImage);
}


void ImagePainter::Discard() {
  delete this;
}


LPVOID ImagePainter::AddPane(const IPane*) {
  return nullptr;
}


HRESULT ImagePainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  ID2D1Bitmap *bitmap;
  mRenderTarget = renderTarget;
  if (mImage) {
    if (SUCCEEDED(renderTarget->CreateBitmapFromWicBitmap(mImage, &bitmap))) {
      if (SUCCEEDED(renderTarget->CreateBitmapBrush(bitmap, &mBrush))) {
        mBrush->SetTransform(GetTransform());
      }
      bitmap->Release();
    }
  }
  return S_OK;
}


void ImagePainter::DiscardDeviceResources() {
  SAFERELEASE(mBrush);
}


bool ImagePainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void ImagePainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane*, LPVOID, UINT) const {
  D2D1_RECT_F intersection;
  if (mBrush != nullptr && RectIntersection(area, &mPaintingPosition, &intersection)) {
    renderTarget->FillRectangle(intersection, mBrush);
  }
}


void ImagePainter::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void ImagePainter::PositionChanged(const IPane *pane, LPVOID, const D2D1_RECT_F&, bool, bool) {
  mPaintingPosition = pane->EvaluateRect(mPosition);
  mPaintingPosition.left += pane->GetRenderingPosition().left;
  mPaintingPosition.top += pane->GetRenderingPosition().top;
  mPaintingPosition.right += pane->GetRenderingPosition().left;
  mPaintingPosition.bottom += pane->GetRenderingPosition().top;
  if (mBrush) {
    mBrush->SetTransform(GetTransform());
  }
}


void ImagePainter::RemovePane(const IPane*, LPVOID) {}


void ImagePainter::TextChanged(const IPane*, LPVOID, LPCWSTR) {}


void ImagePainter::SetPosition(NRECT position, const IPane *pane) {
  mPosition = position;
  if (pane) {
    mPaintingPosition = pane->EvaluateRect(mPosition);
    mPaintingPosition.left += pane->GetRenderingPosition().left;
    mPaintingPosition.top += pane->GetRenderingPosition().top;
    mPaintingPosition.right += pane->GetRenderingPosition().left;
    mPaintingPosition.bottom += pane->GetRenderingPosition().top;
  }
  if (mBrush) {
    mBrush->SetTransform(GetTransform());
  }
}


void ImagePainter::SetImage(HICON icon) {
  IWICImagingFactory *factory = GetWICFactory();
  IWICBitmap *wicBitmap = nullptr;
  factory->CreateBitmapFromHICON(icon, &wicBitmap);
  SetImage(wicBitmap);
  if (wicBitmap) {
    wicBitmap->Release();
  }
}


void ImagePainter::SetImage(HBITMAP bitmap) {
  IWICImagingFactory *factory = GetWICFactory();
  IWICBitmap *wicBitmap = nullptr;
  factory->CreateBitmapFromHBITMAP(bitmap, nullptr, WICBitmapUseAlpha, &wicBitmap);
  SetImage(wicBitmap);
  if (wicBitmap) {
    wicBitmap->Release();
  }
}


void ImagePainter::SetImage(IWICBitmapSource *image) {
  SAFERELEASE(mImage);
  IWICImagingFactory *factory = GetWICFactory();
  IWICFormatConverter *converter = nullptr;
  if (SUCCEEDED(factory->CreateFormatConverter(&converter))) {
    if (SUCCEEDED(converter->Initialize(image, GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeMedianCut))) {
      mImage = converter;
      mImage->GetSize(&mImageSize.width, &mImageSize.height);
    }
  }
  if (mRenderTarget) {
    ID2D1RenderTarget *renderTarget = mRenderTarget;
    DiscardDeviceResources();
    CreateDeviceResources(renderTarget);
  }
}


D2D1_MATRIX_3X2_F ImagePainter::GetTransform() const {
  return D2D1::Matrix3x2F::Scale(
    (mPaintingPosition.right - mPaintingPosition.left) / (float)mImageSize.width,
    (mPaintingPosition.bottom - mPaintingPosition.top) / (float)mImageSize.height)
    * D2D1::Matrix3x2F::Translation(mPaintingPosition.left, mPaintingPosition.top);
}
