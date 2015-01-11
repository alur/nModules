/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Brush.cpp
*  The nModules Project
*
*  A general brush.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "Brush.hpp"
#include "Color.h"
#include "Factories.h"
#include "LiteStep.h"
#include <wincodec.h>
#include "../nCoreCom/Core.h"
#include "../Utilities/StringUtils.h"
#include <algorithm>
#include "ErrorHandler.h"
#include <Shlwapi.h>


Brush::Brush()
    : brush(nullptr)
    , brushSettings(nullptr)
    , brushType(Type::SolidColor)
    , gradientStopColors(nullptr)
    , gradientStopCount(0)
    , gradientStops(nullptr)
    , mTransformTimeStamp(0)
    , scalingMode(ImageScalingMode::Center)
{}


Brush::~Brush() {
  Discard();
  for (UINT i = 0; i < this->gradientStopCount; ++i) {
    delete this->gradientStopColors[i];
  }
  SAFEFREE(this->gradientStopColors);
  SAFEFREE(this->gradientStops);
}


void Brush::Load(BrushSettings* settings) {
  this->brushSettings = settings;

  if (_wcsicmp(this->brushSettings->brushType, L"Image") == 0) {
    this->brushType = Type::Image;
  } else if (_wcsicmp(this->brushSettings->brushType, L"LinearGradient") == 0) {
    this->brushType = Type::LinearGradient;
    this->gradientStart = D2D1::Point2F(this->brushSettings->gradientStartX,
        this->brushSettings->gradientStartY);
    this->gradientEnd = D2D1::Point2F(this->brushSettings->gradientEndX,
        this->brushSettings->gradientEndY);
    LoadGradientStops();
  } else if (_wcsicmp(this->brushSettings->brushType, L"RadialGradient") == 0) {
    this->brushType = Type::RadialGradient;
    this->gradientCenter = D2D1::Point2F(this->brushSettings->gradientCenterX,
        this->brushSettings->gradientCenterY);
    this->gradientOriginOffset = D2D1::Point2F(this->brushSettings->gradientOriginOffsetX,
        this->brushSettings->gradientOriginOffsetY);
    LoadGradientStops();
  } else {
    this->brushType = Type::SolidColor;
  }

  if (_wcsicmp(this->brushSettings->imageScalingMode, L"Tile") == 0) {
    this->scalingMode = ImageScalingMode::Tile;
  } else if (_wcsicmp(this->brushSettings->imageScalingMode, L"Fit") == 0) {
    this->scalingMode = ImageScalingMode::Fit;
  } else if (_wcsicmp(this->brushSettings->imageScalingMode, L"Fill") == 0) {
    this->scalingMode = ImageScalingMode::Fill;
  } else if (_wcsicmp(this->brushSettings->imageScalingMode, L"Stretch") == 0) {
    this->scalingMode = ImageScalingMode::Stretch;
  } else if (_wcsicmp(this->brushSettings->imageScalingMode, L"Edges") == 0) {
    this->scalingMode = ImageScalingMode::Edges;
  } else {
    this->scalingMode = ImageScalingMode::Center;
  }

  if (_wcsicmp(this->brushSettings->tilingModeX, L"Mirror") == 0) {
    this->tileModeX = D2D1_EXTEND_MODE_MIRROR;
  } else if (_wcsicmp(this->brushSettings->tilingModeX, L"Clamp") == 0) {
    this->tileModeX = D2D1_EXTEND_MODE_CLAMP;
  } else {
    this->tileModeX = D2D1_EXTEND_MODE_WRAP;
  }

  if (_wcsicmp(this->brushSettings->tilingModeY, L"Mirror") == 0) {
    this->tileModeY = D2D1_EXTEND_MODE_MIRROR;
  } else if (_wcsicmp(this->brushSettings->tilingModeY, L"Clamp") == 0) {
    this->tileModeY = D2D1_EXTEND_MODE_CLAMP;
  } else {
    this->tileModeY = D2D1_EXTEND_MODE_WRAP;
  }

  this->imageEdges = brushSettings->imageEdges;
}


void Brush::LoadGradientStops() {
  WCHAR colorToken[MAX_LINE_LENGTH], stopToken[MAX_LINE_LENGTH];
  LPCWSTR colorPointer = this->brushSettings->gradientColors,
          stopPointer = this->brushSettings->gradientStops;

  using namespace LiteStep;

  std::unique_ptr<IColorVal> defaultColor = Color::Create(0xFF000000);

  while (GetToken(colorPointer, colorToken, &colorPointer, FALSE) != FALSE &&
      GetToken(stopPointer, stopToken, &stopPointer, FALSE) != FALSE) {
    float stop;
    LPWSTR endPtr;

    IColorVal *color = ParseColor(colorToken, defaultColor.get());
    stop = wcstof(stopToken, &endPtr);

    this->gradientStops = (D2D1_GRADIENT_STOP*)realloc(this->gradientStops,
        ++this->gradientStopCount*sizeof(D2D1_GRADIENT_STOP));
    this->gradientStopColors = (IColorVal**)realloc(this->gradientStopColors,
        this->gradientStopCount*sizeof(IColorVal*));
    this->gradientStopColors[this->gradientStopCount - 1] = color;
    this->gradientStops[this->gradientStopCount - 1].color = Color::ARGBToD2D(color->Evaluate());
    this->gradientStops[this->gradientStopCount - 1].position = stop;
  }
}


void Brush::Discard() {
  SAFERELEASE(this->brush);
}


HRESULT Brush::ReCreate(ID2D1RenderTarget* renderTarget) {
  HRESULT hr = S_OK;

  if (renderTarget) {
    SAFERELEASE(this->brush);

    switch (this->brushType) {
    case Type::SolidColor:
      {
        hr = renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings->color->Evaluate()), (ID2D1SolidColorBrush**)&this->brush);
      }
      break;

    case Type::LinearGradient:
      {
        ID2D1GradientStopCollection *stops;
        hr = renderTarget->CreateGradientStopCollection(this->gradientStops, this->gradientStopCount, &stops);
        if (SUCCEEDED(hr)) {
          hr = renderTarget->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(this->gradientStart, this->gradientEnd),
            stops, reinterpret_cast<ID2D1LinearGradientBrush**>(&this->brush));
          stops->Release();
        }
      }
      break;

    case Type::RadialGradient:
      {
        ID2D1GradientStopCollection* stops;
        hr = renderTarget->CreateGradientStopCollection(this->gradientStops, this->gradientStopCount, &stops);
        if (SUCCEEDED(hr)) {
          hr = renderTarget->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(this->gradientCenter, this->gradientOriginOffset,
            this->brushSettings->gradientRadiusX, this->brushSettings->gradientRadiusY),
            stops, reinterpret_cast<ID2D1RadialGradientBrush**>(&this->brush));
          stops->Release();
        }
      }
      break;

    case Type::Image:
      {
        if (SUCCEEDED(hr = LoadImageFile(renderTarget, this->brushSettings->image, &this->brush))) {
          this->brush->SetOpacity(this->brushSettings->imageOpacity);
        } else {
          // Happens a bit too much...
          //ErrorHandler::ErrorHR(ErrorHandler::Level::Warning, hr, L"Failed to load %s", this->brushSettings->image);
          // It's cool.
          hr = S_FALSE;
        }
      }
      break;
    }
  }

  return hr;
}


bool Brush::IsImageEdgeBrush() const {
  return brushType == Type::Image && scalingMode == ImageScalingMode::Edges;
}


D2D1_RECT_F *Brush::GetImageEdgeRectAndScaleBrush(EdgeType edgeType, WindowData *windowData) {
  this->brush->SetTransform(windowData->imageEdgeTransforms[edgeType]);
  return &windowData->imageEdgeRects[edgeType];
}


void Brush::ComputeEdgeData(D2D1_SIZE_F size, WindowData *windowData) {
  using namespace D2D1;

  D2D1_RECT_F imageEdges = this->imageEdges;

  // If the window isn't big enough, trim the edges.
  if (imageEdges.left + imageEdges.right > size.width) {
    float scale = size.width / (imageEdges.left + imageEdges.right);
    imageEdges.left *= scale;
    imageEdges.right *= scale;
  }
  if (imageEdges.bottom + imageEdges.top > size.height) {
    float scale = size.height / (imageEdges.bottom + imageEdges.top);
    imageEdges.bottom *= scale;
    imageEdges.top *= scale;
  }

  windowData->imageEdgeRects[EdgeType::TopLeft] = D2D1::Rect(
    windowData->position.left,
    windowData->position.top,
    windowData->position.left + imageEdges.left,
    windowData->position.top + imageEdges.top);

  windowData->imageEdgeRects[EdgeType::TopCenter] = D2D1::Rect(
    windowData->position.left + imageEdges.left,
    windowData->position.top,
    windowData->position.right - imageEdges.right,
    windowData->position.top + imageEdges.top);

  windowData->imageEdgeRects[EdgeType::TopRight] = D2D1::Rect(
    windowData->position.right - imageEdges.right,
    windowData->position.top,
    windowData->position.right,
    windowData->position.top + imageEdges.top);

  windowData->imageEdgeRects[EdgeType::MiddleLeft] = D2D1::Rect(
    windowData->position.left,
    windowData->position.top + imageEdges.top,
    windowData->position.left + imageEdges.left,
    windowData->position.bottom - imageEdges.bottom);

  windowData->imageEdgeRects[EdgeType::MiddleCenter] = D2D1::Rect(
    windowData->position.left + imageEdges.left,
    windowData->position.top + imageEdges.top,
    windowData->position.right - imageEdges.right,
    windowData->position.bottom - imageEdges.bottom);

  windowData->imageEdgeRects[EdgeType::MiddleRight] = D2D1::Rect(
    windowData->position.right - imageEdges.right,
    windowData->position.top + imageEdges.top,
    windowData->position.right,
    windowData->position.bottom - imageEdges.bottom);

  windowData->imageEdgeRects[EdgeType::BottomLeft] = D2D1::Rect(
    windowData->position.left,
    windowData->position.bottom - imageEdges.bottom,
    windowData->position.left + imageEdges.left,
    windowData->position.bottom);

  windowData->imageEdgeRects[EdgeType::BottomCenter] = D2D1::Rect(
    windowData->position.left + imageEdges.left,
    windowData->position.bottom - imageEdges.bottom,
    windowData->position.right - imageEdges.right,
    windowData->position.bottom);

  windowData->imageEdgeRects[EdgeType::BottomRight] = D2D1::Rect(
    windowData->position.right - imageEdges.right,
    windowData->position.bottom - imageEdges.bottom,
    windowData->position.right,
    windowData->position.bottom);

  windowData->imageEdgeTransforms[EdgeType::TopLeft] = Matrix3x2F::Translation(windowData->position.left, windowData->position.top);
  windowData->imageEdgeTransforms[EdgeType::TopRight] = Matrix3x2F::Translation(windowData->position.right - size.width, windowData->position.top);
  windowData->imageEdgeTransforms[EdgeType::BottomLeft] = Matrix3x2F::Translation(windowData->position.left, windowData->position.bottom - size.height);
  windowData->imageEdgeTransforms[EdgeType::BottomRight] = Matrix3x2F::Translation(windowData->position.right - size.width, windowData->position.bottom - size.height);

  windowData->imageEdgeTransforms[EdgeType::TopCenter] = Matrix3x2F::Translation(windowData->position.left, windowData->position.top) *
    Matrix3x2F::Scale(
    (windowData->position.right - windowData->position.left) / (size.width - imageEdges.left - imageEdges.right),
    1.0f,
    Point2F(windowData->position.left + imageEdges.left, windowData->position.top)
    );

  windowData->imageEdgeTransforms[EdgeType::MiddleLeft] = Matrix3x2F::Translation(windowData->position.left, windowData->position.top) *
    Matrix3x2F::Scale(
    1.0f,
    (windowData->position.bottom - windowData->position.top) / (size.height - imageEdges.top - imageEdges.bottom),
    Point2F(windowData->position.left, windowData->position.top + imageEdges.top)
    );

  windowData->imageEdgeTransforms[EdgeType::MiddleRight] = Matrix3x2F::Translation(windowData->position.right - size.width, windowData->position.top) *
    Matrix3x2F::Scale(
    1.0f,
    (windowData->position.bottom - windowData->position.top) / (size.height - imageEdges.top - imageEdges.bottom),
    Point2F(windowData->position.right, windowData->position.top + imageEdges.top)
    );

  windowData->imageEdgeTransforms[EdgeType::BottomCenter] = Matrix3x2F::Translation(windowData->position.left, windowData->position.bottom - size.height) *
    Matrix3x2F::Scale(
    (windowData->position.right - windowData->position.left) / (size.width - imageEdges.left - imageEdges.right),
    1.0f,
    Point2F(windowData->position.left + imageEdges.left, windowData->position.bottom)
    );

  windowData->imageEdgeTransforms[EdgeType::MiddleCenter] = Matrix3x2F::Translation(windowData->position.left, windowData->position.top) *
    Matrix3x2F::Scale(
    (windowData->position.right - windowData->position.left) / (size.width - imageEdges.left - imageEdges.right),
    (windowData->position.bottom - windowData->position.top) / (size.height - imageEdges.top - imageEdges.bottom),
    Point2F(windowData->position.left + imageEdges.left, windowData->position.top + imageEdges.top)
    );
}


void Brush::ScaleImage(WindowData *windowData) {
  ID2D1Bitmap* bitmap;
  reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->GetBitmap(&bitmap);
  D2D1_SIZE_F size = bitmap->GetSize();
  bitmap->Release();

  using namespace D2D1;
  using std::min;
  using std::max;

  D2D1_POINT_2F centerPoint = Point2F(
    windowData->position.left + (windowData->position.right - windowData->position.left) / 2.0f,
    windowData->position.top + (windowData->position.bottom - windowData->position.top) / 2.0f
    );

  switch (this->scalingMode) {
  case ImageScalingMode::Center:
    {
      windowData->brushTransform = Matrix3x2F::Translation(centerPoint.x - size.width / 2.0f, centerPoint.y - size.height / 2.0f);
      windowData->brushPosition.left = max(centerPoint.x - size.width / 2.0f, windowData->position.left);
      windowData->brushPosition.top = max(centerPoint.y - size.height / 2.0f, windowData->position.top);
      windowData->brushPosition.right = min(windowData->brushPosition.left + size.width, windowData->position.right);
      windowData->brushPosition.bottom = min(windowData->brushPosition.top + size.height, windowData->position.bottom);
    }
    break;

  case ImageScalingMode::Stretch:
    {
      windowData->brushTransform =
        Matrix3x2F::Translation(windowData->position.left, windowData->position.top) *
        Matrix3x2F::Scale(
        (windowData->position.right - windowData->position.left) / size.width,
        (windowData->position.bottom - windowData->position.top) / size.height,
        Point2F(windowData->position.left, windowData->position.top)
        );
    }
    break;

  case ImageScalingMode::Fit:
    {
      float scale = min((windowData->position.right - windowData->position.left) / size.width, (windowData->position.bottom - windowData->position.top) / size.height);
      windowData->brushTransform =
        Matrix3x2F::Translation(centerPoint.x - size.width / 2.0f, centerPoint.y - size.height / 2.0f) *
        Matrix3x2F::Scale(scale, scale, centerPoint);
      windowData->brushPosition.left = centerPoint.x - scale*size.width / 2.0f;
      windowData->brushPosition.top = centerPoint.y - scale*size.height / 2.0f;
      windowData->brushPosition.right = windowData->brushPosition.left + scale*size.width;
      windowData->brushPosition.bottom = windowData->brushPosition.top + scale*size.height;
    }
    break;

  case ImageScalingMode::Fill:
    {
      float scale = max((windowData->position.right - windowData->position.left) / size.width, (windowData->position.bottom - windowData->position.top) / size.height);
      windowData->brushTransform =
        Matrix3x2F::Translation(centerPoint.x - size.width / 2.0f, centerPoint.y - size.height / 2.0f) *
        Matrix3x2F::Scale(scale, scale, centerPoint);
    }
    break;

  case ImageScalingMode::Tile:
    {
      windowData->brushTransform = D2D1::Matrix3x2F::Translation(windowData->position.left, windowData->position.top);
      reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeX(this->tileModeX);
      reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeY(this->tileModeY);
    }
    break;

  case ImageScalingMode::Edges:
    {
      ComputeEdgeData(size, windowData);
    }
    break;
  }
}


void Brush::UpdatePosition(D2D1_RECT_F position, WindowData *windowData) {
  windowData->position = windowData->brushPosition = position;

  if (this->brush) {
    UpdateTransform(windowData);
  }
}


void Brush::UpdateTransform(WindowData *windowData) {
  if (this->brushType == Type::Image) {
    ScaleImage(windowData);
  } else {
    windowData->brushTransform =
      D2D1::Matrix3x2F::Translation(windowData->position.left, windowData->position.top);
  }
}


bool Brush::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget) {
  bool ret = false;

  switch (this->brushType) {
  case Type::SolidColor:
    {
      if (!this->brushSettings->color->IsConstant() && this->brush) {
        ((ID2D1SolidColorBrush*) this->brush)->SetColor(
          Color::ARGBToD2D(this->brushSettings->color->Evaluate(newColor)));
        ret = true;
      }
    }
    break;

  case Type::LinearGradient:
  case Type::RadialGradient:
    {
      for (UINT i = 0; i < this->gradientStopCount; ++i) {
        if (!this->gradientStopColors[i]->IsConstant()) {
          this->gradientStops[i].color =
            Color::ARGBToD2D(this->gradientStopColors[i]->Evaluate(newColor));
          ret = true;
        }
      }

      if (ret) {
        ReCreate(renderTarget);
      }
    }
    break;
  }

  return ret;
}


HRESULT Brush::LoadImageFile(ID2D1RenderTarget *renderTarget, LPCTSTR image, ID2D1Brush **brush) {
  IWICImagingFactory *factory = nullptr;
  IWICBitmap *wicBitmap = nullptr;
  IWICFormatConverter *converter = nullptr;
  ID2D1Bitmap *bitmap = nullptr;
  Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
  HRESULT hr = E_FAIL;

  HBITMAP hBitmap = LiteStep::LoadLSImage(image, nullptr);
  if (hBitmap) {
    hr = factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(hr)) {
      hr = factory->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapUseAlpha, &wicBitmap);
    }
    if (SUCCEEDED(hr)) {
      hr = converter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
        nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
    }
    if (SUCCEEDED(hr)) {
      hr = renderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    }
    if (SUCCEEDED(hr)) {
      hr = renderTarget->CreateBitmapBrush(bitmap, reinterpret_cast<ID2D1BitmapBrush**>(brush));
    }

    DeleteObject(hBitmap);
    SAFERELEASE(wicBitmap);
    SAFERELEASE(bitmap);
    SAFERELEASE(converter);
  } else if (!PathFileExists(image)) {
    hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
  }

  return hr;
}


void Brush::SetColor(const IColorVal *color) {
  this->brushSettings->color = std::unique_ptr<IColorVal>(color->Copy());
  if (this->brushType == Type::SolidColor) {
    if (this->brush) {
      ((ID2D1SolidColorBrush*) this->brush)->SetColor(
        Color::ARGBToD2D(this->brushSettings->color->Evaluate()));
    }
  }
}


void Brush::SetImage(ID2D1RenderTarget *renderTarget, LPCTSTR path) {
  this->brushSettings->image = StringUtils::ReallocOverwrite(this->brushSettings->image, path);

  if (this->brushType == Type::Image && renderTarget != nullptr) {
    ID2D1Brush *tempBrush;
    if (SUCCEEDED(LoadImageFile(renderTarget, path, &tempBrush))) {
      SAFERELEASE(this->brush);
      this->brush = tempBrush;
      this->brush->SetOpacity(this->brushSettings->imageOpacity);
      mTransformTimeStamp = GetTickCount64();
    }
  }
}


void Brush::CheckTransforms(WindowData *wndData) {
  if (wndData->transformComputationTime != mTransformTimeStamp) {
    UpdateTransform(wndData);
    wndData->transformComputationTime = mTransformTimeStamp;
  }
}


BrushSettings *Brush::GetBrushSettings() {
  return brushSettings;
}
