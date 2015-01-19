//-------------------------------------------------------------------------------------------------
// /nShared/Overlay.cpp
// The nModules Project
//
// An overlay for a drawable window. An image or an icon.
//-------------------------------------------------------------------------------------------------
#include "Factories.h"
#include "LiteStep.h"
#include "Overlay.hpp"

#include "../Utilities/CommonD2D.h"

#include <Wincodec.h>

using namespace D2D1;


Overlay::Overlay(D2D1_RECT_F position, D2D1_RECT_F parentPosition, IWICBitmapSource* source, int zOrder) {
  this->position = position;
  this->source = source;
  this->brush = nullptr;
  this->renderTarget = nullptr;
  this->zOrder = zOrder;
  UpdatePosition(parentPosition);
}


Overlay::~Overlay() {
  DiscardDeviceResources();
  SAFERELEASE(this->source);
}


HRESULT Overlay::ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) {
  IWICBitmapScaler* scaler = NULL;
  IWICFormatConverter* converter = NULL;
  IWICImagingFactory* factory = NULL;
  ID2D1Bitmap* bitmap = NULL;
  UINT width, height;

  HRESULT hr = S_OK;

  this->renderTarget = renderTarget;

  if (renderTarget != NULL && this->source != NULL) {
    ASSERT(this->brush == NULL);

    // Create our helper objects.
    hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
    if (SUCCEEDED(hr)) {
      hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
      hr = factory->CreateFormatConverter(&converter);
    }

    // Resize the source
    if (SUCCEEDED(hr)) {
      hr = source->GetSize(&width, &height);
    }
    if (SUCCEEDED(hr)) {
      hr = scaler->Initialize(source, (UINT)(position.right - position.left), (UINT)(position.bottom - position.top), WICBitmapInterpolationModeCubic);
    }

    // Convert it to an ID2D1Bitmap
    if (SUCCEEDED(hr)) {
      hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
    }
    if (SUCCEEDED(hr)) {
      hr = renderTarget->CreateBitmapFromWicBitmap(converter, 0, &bitmap);
    }

    // Create a brush based on the bitmap
    if (SUCCEEDED(hr)) {
      hr = renderTarget->CreateBitmapBrush(bitmap, &this->brush);
    }

    // Move the origin of the brush to match the overlay position
    if (SUCCEEDED(hr)) {
      this->brush->SetTransform(Matrix3x2F::Translation(this->drawingPosition.left, this->drawingPosition.top));
    }

    // Release stuff
    SAFERELEASE(scaler);
    SAFERELEASE(converter);
    SAFERELEASE(bitmap);
  }

  return hr;
}


void Overlay::DiscardDeviceResources() {
  SAFERELEASE(this->brush);
  this->renderTarget = nullptr;
}


void Overlay::UpdatePosition(D2D1_RECT_F parentPosition) {
  this->drawingPosition = this->position;
  this->drawingPosition.left += parentPosition.left;
  this->drawingPosition.right += parentPosition.left;
  this->drawingPosition.top += parentPosition.top;
  this->drawingPosition.bottom += parentPosition.top;

  // Move the origin of the brush to match the overlay position
  if (this->brush) {
    this->brush->SetTransform(Matrix3x2F::Identity());
    this->brush->SetTransform(Matrix3x2F::Translation(this->drawingPosition.left, this->drawingPosition.top));
  }
}


void Overlay::Paint(ID2D1RenderTarget* renderTarget) {
  if (this->brush != NULL) {
    renderTarget->FillRectangle(this->drawingPosition, this->brush);
  }
}


void Overlay::SetSource(IWICBitmapSource* source) {
  SAFERELEASE(this->brush);
  SAFERELEASE(this->source);
  this->source = source;
  ReCreateDeviceResources(this->renderTarget);
}


ID2D1BitmapBrush *Overlay::GetBrush() {
  return brush;
}


int Overlay::GetZOrder() {
  return this->zOrder;
}


bool Overlay::UpdateDWMColor(ARGB, ID2D1RenderTarget*) {
  return false;
}
