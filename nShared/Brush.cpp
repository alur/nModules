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


Brush::Brush()
{
    this->brushType = Type::SolidColor;
    this->brush = nullptr;
    this->gradientStops = nullptr;
    this->gradientStopColors = nullptr;
    this->gradientStopCount = 0;
}


Brush::~Brush()
{
    Discard();
    for (UINT i = 0; i < this->gradientStopCount; ++i)
    {
        delete this->gradientStopColors[i];
    }
    SAFEFREE(this->gradientStopColors);
    SAFEFREE(this->gradientStops);
}


void Brush::Load(BrushSettings* settings)
{
    this->brushSettings = settings;

    if (_tcsicmp(this->brushSettings->brushType, _T("Image")) == 0)
    {
        this->brushType = Type::Image;
    }
    else if (_tcsicmp(this->brushSettings->brushType, _T("LinearGradient")) == 0)
    {
        this->brushType = Type::LinearGradient;
        this->gradientStart = D2D1::Point2F(this->brushSettings->gradientStartX, this->brushSettings->gradientStartY);
        this->gradientEnd = D2D1::Point2F(this->brushSettings->gradientEndX, this->brushSettings->gradientEndY);
        LoadGradientStops();
    }
    else if (_tcsicmp(this->brushSettings->brushType, _T("RadialGradient")) == 0)
    {
        this->brushType = Type::RadialGradient;
        this->gradientCenter = D2D1::Point2F(this->brushSettings->gradientCenterX, this->brushSettings->gradientCenterY);
        this->gradientOriginOffset = D2D1::Point2F(this->brushSettings->gradientOriginOffsetX, this->brushSettings->gradientOriginOffsetY);
        LoadGradientStops();
    }
    else
    {
        this->brushType = Type::SolidColor;
    }

    if (_tcsicmp(this->brushSettings->imageScalingMode, _T("Tile")) == 0)
    {
        this->scalingMode = ImageScalingMode::Tile;
    }
    else if (_tcsicmp(this->brushSettings->imageScalingMode, _T("Fit")) == 0)
    {
        this->scalingMode = ImageScalingMode::Fit;
    }
    else if (_tcsicmp(this->brushSettings->imageScalingMode, _T("Fill")) == 0)
    {
        this->scalingMode = ImageScalingMode::Fill;
    }
    else if (_tcsicmp(this->brushSettings->imageScalingMode, _T("Stretch")) == 0)
    {
        this->scalingMode = ImageScalingMode::Stretch;
    }
    else if (_tcsicmp(this->brushSettings->imageScalingMode, _T("Edges")) == 0)
    {
        this->scalingMode = ImageScalingMode::Edges;
    }
    else
    {
        this->scalingMode = ImageScalingMode::Center;
    }

    if (_tcsicmp(this->brushSettings->tilingModeX, _T("Mirror")) == 0)
    {
        this->tileModeX = D2D1_EXTEND_MODE_MIRROR;
    }
    else if (_tcsicmp(this->brushSettings->tilingModeX, _T("Clamp")) == 0)
    {
        this->tileModeX = D2D1_EXTEND_MODE_CLAMP;
    }
    else
    {
        this->tileModeX = D2D1_EXTEND_MODE_WRAP;
    }

    if (_tcsicmp(this->brushSettings->tilingModeY, _T("Mirror")) == 0)
    {
        this->tileModeY = D2D1_EXTEND_MODE_MIRROR;
    }
    else if (_tcsicmp(this->brushSettings->tilingModeY, _T("Clamp")) == 0)
    {
        this->tileModeY = D2D1_EXTEND_MODE_CLAMP;
    }
    else
    {
        this->tileModeY = D2D1_EXTEND_MODE_WRAP;
    }

    this->imageEdges = brushSettings->imageEdges;
}


void Brush::LoadGradientStops()
{
    TCHAR colorToken[MAX_LINE_LENGTH], stopToken[MAX_LINE_LENGTH];
    LPCTSTR colorPointer = this->brushSettings->gradientColors, stopPointer = this->brushSettings->gradientStops;

    using namespace LiteStep;

    std::unique_ptr<IColorVal> defaultColor = Color::Create(0xFF000000);

    while (GetToken(colorPointer, colorToken, &colorPointer, FALSE) != FALSE && GetToken(stopPointer, stopToken, &stopPointer, FALSE) != FALSE)
    {
        float stop;
        LPTSTR endPtr;

        IColorVal *color = ParseColor(colorToken, defaultColor.get());
        stop = (float)_tcstod(stopToken, &endPtr);

        this->gradientStops = (D2D1_GRADIENT_STOP*)realloc(this->gradientStops, ++this->gradientStopCount*sizeof(D2D1_GRADIENT_STOP));
        this->gradientStopColors = (IColorVal**)realloc(this->gradientStopColors, this->gradientStopCount*sizeof(IColorVal*));
        this->gradientStopColors[this->gradientStopCount - 1] = color;
        this->gradientStops[this->gradientStopCount - 1].color = Color::ARGBToD2D(color->Evaluate());
        this->gradientStops[this->gradientStopCount - 1].position = stop;
    }
}


//
void Brush::Discard()
{
    SAFERELEASE(this->brush);
}


//
HRESULT Brush::ReCreate(ID2D1RenderTarget* renderTarget)
{
    HRESULT hr = S_OK;

    if (renderTarget)
    {
        SAFERELEASE(this->brush);

        switch (this->brushType)
        {
        case Type::SolidColor:
            {
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings->color->Evaluate()), (ID2D1SolidColorBrush**)&this->brush);
            }
            break;

        case Type::LinearGradient:
            {
                ID2D1GradientStopCollection* stops;
                renderTarget->CreateGradientStopCollection(this->gradientStops, this->gradientStopCount, &stops);
                renderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(this->gradientStart, this->gradientEnd),
                    stops, reinterpret_cast<ID2D1LinearGradientBrush**>(&this->brush));
                stops->Release();
            }
            break;

        case Type::RadialGradient:
            {
                ID2D1GradientStopCollection* stops;
                renderTarget->CreateGradientStopCollection(this->gradientStops, this->gradientStopCount, &stops);
                renderTarget->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(this->gradientCenter, this->gradientOriginOffset,
                    this->brushSettings->gradientRadiusX, this->brushSettings->gradientRadiusY),
                    stops, reinterpret_cast<ID2D1RadialGradientBrush**>(&this->brush));
                stops->Release();
            }
            break;

        case Type::Image:
            {
                if (SUCCEEDED(hr = LoadImageFile(renderTarget, this->brushSettings->image, &this->brush))) {
                    this->brush->SetOpacity(this->brushSettings->imageOpacity);
                }
            }
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        UpdatePosition(this->position);
    }

    return hr;
}


bool Brush::IsImageEdgeBrush() const
{
    return brushType == Type::Image && scalingMode == ImageScalingMode::Edges;
}


D2D1_RECT_F *Brush::GetImageEdgeRectAndScaleBrush(EdgeType edgeType)
{
    this->brush->SetTransform(mImageEdgeTransforms[edgeType]);
    return &mImageEdgeRects[edgeType];
}


void Brush::ComputeEdgeData(D2D1_SIZE_F size)
{
    using namespace D2D1;

    D2D1_RECT_F imageEdges = this->imageEdges;

    // If the window isn't big enough, trim the edges.
    if (imageEdges.left + imageEdges.right > size.width)
    {
        float scale = size.width / (imageEdges.left + imageEdges.right);
        imageEdges.left *= scale;
        imageEdges.right *= scale;
    }
    if (imageEdges.bottom + imageEdges.top > size.height)
    {
        float scale = size.height / (imageEdges.bottom + imageEdges.top);
        imageEdges.bottom *= scale;
        imageEdges.top *= scale;
    }

    mImageEdgeRects[EdgeType::TopLeft] = D2D1::Rect(
        position.left,
        position.top,
        position.left + imageEdges.left,
        position.top + imageEdges.top);

    mImageEdgeRects[EdgeType::TopCenter] = D2D1::Rect(
        position.left + imageEdges.left,
        position.top,
        position.right - imageEdges.right,
        position.top + imageEdges.top);

    mImageEdgeRects[EdgeType::TopRight] = D2D1::Rect(
        position.right - imageEdges.right,
        position.top,
        position.right,
        position.top + imageEdges.top);

    mImageEdgeRects[EdgeType::MiddleLeft] = D2D1::Rect(
        position.left,
        position.top + imageEdges.top,
        position.left + imageEdges.left,
        position.bottom - imageEdges.bottom);

    mImageEdgeRects[EdgeType::MiddleCenter] = D2D1::Rect(
        position.left + imageEdges.left,
        position.top + imageEdges.top,
        position.right - imageEdges.right,
        position.bottom - imageEdges.bottom);

    mImageEdgeRects[EdgeType::MiddleRight] = D2D1::Rect(
        position.right - imageEdges.right,
        position.top + imageEdges.top,
        position.right,
        position.bottom - imageEdges.bottom);

    mImageEdgeRects[EdgeType::BottomLeft] = D2D1::Rect(
        position.left,
        position.bottom - imageEdges.bottom,
        position.left + imageEdges.left,
        position.bottom);

    mImageEdgeRects[EdgeType::BottomCenter] = D2D1::Rect(
        position.left + imageEdges.left,
        position.bottom - imageEdges.bottom,
        position.right - imageEdges.right,
        position.bottom);

    mImageEdgeRects[EdgeType::BottomRight] = D2D1::Rect(
        position.right - imageEdges.right,
        position.bottom - imageEdges.bottom,
        position.right,
        position.bottom);

    mImageEdgeTransforms[EdgeType::TopLeft] = Matrix3x2F::Translation(this->position.left, this->position.top);
    mImageEdgeTransforms[EdgeType::TopRight] = Matrix3x2F::Translation(this->position.right - size.width, this->position.top);
    mImageEdgeTransforms[EdgeType::BottomLeft] = Matrix3x2F::Translation(this->position.left, this->position.bottom - size.height);
    mImageEdgeTransforms[EdgeType::BottomRight] = Matrix3x2F::Translation(this->position.right - size.width, this->position.bottom - size.height);

    mImageEdgeTransforms[EdgeType::TopCenter] = Matrix3x2F::Translation(this->position.left, this->position.top) *
        Matrix3x2F::Scale(
            (position.right - position.left) / (size.width - imageEdges.left - imageEdges.right),
            1.0f,
            Point2F(this->position.left + imageEdges.left, this->position.top)
        );

    mImageEdgeTransforms[EdgeType::MiddleLeft] = Matrix3x2F::Translation(this->position.left, this->position.top) *
        Matrix3x2F::Scale(
            1.0f,
            (position.bottom - position.top) / (size.height - imageEdges.top - imageEdges.bottom),
            Point2F(this->position.left, this->position.top + imageEdges.top)
        );

    mImageEdgeTransforms[EdgeType::MiddleRight] = Matrix3x2F::Translation(this->position.right - size.width, this->position.top) *
        Matrix3x2F::Scale(
            1.0f,
            (position.bottom - position.top) / (size.height - imageEdges.top - imageEdges.bottom),
            Point2F(this->position.right, this->position.top + imageEdges.top)
        );

    mImageEdgeTransforms[EdgeType::BottomCenter] = Matrix3x2F::Translation(this->position.left, this->position.bottom - size.height) *
        Matrix3x2F::Scale(
            (position.right - position.left) / (size.width - imageEdges.left - imageEdges.right),
            1.0f,
            Point2F(this->position.left + imageEdges.left, this->position.bottom)
        );

    mImageEdgeTransforms[EdgeType::MiddleCenter] = Matrix3x2F::Translation(this->position.left, this->position.top) *
        Matrix3x2F::Scale(
            (this->position.right - this->position.left) / (size.width - imageEdges.left - imageEdges.right),
            (this->position.bottom - this->position.top) / (size.height - imageEdges.top - imageEdges.bottom),
            Point2F(this->position.left + imageEdges.left, this->position.top + imageEdges.top)
        );
}


void Brush::ScaleImage()
{
    ID2D1Bitmap* bitmap;
    reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->GetBitmap(&bitmap);
    D2D1_SIZE_F size = bitmap->GetSize();
    bitmap->Release();

    using namespace D2D1;
    using std::min;
    using std::max;

    D2D1_POINT_2F centerPoint = Point2F(
        this->position.left + (this->position.right - this->position.left)/2.0f,
        this->position.top + (this->position.bottom - this->position.top)/2.0f
    );

    switch(this->scalingMode)
    {
    case ImageScalingMode::Center:
        {
            this->brush->SetTransform(Matrix3x2F::Translation(centerPoint.x - size.width/2.0f, centerPoint.y - size.height/2.0f));
            this->brushPosition.left = max(centerPoint.x - size.width/2.0f, this->position.left);
            this->brushPosition.top = max(centerPoint.y - size.height/2.0f, this->position.top);
            this->brushPosition.right = min(this->brushPosition.left + size.width, this->position.right);
            this->brushPosition.bottom = min(this->brushPosition.top + size.height, this->position.bottom);
        }
        break;

    case ImageScalingMode::Stretch:
        {
            this->brush->SetTransform(
                Matrix3x2F::Translation(this->position.left, this->position.top) *
                Matrix3x2F::Scale(
                    (this->position.right - this->position.left)/size.width,
                    (this->position.bottom - this->position.top)/size.height,
                    Point2F(this->position.left, this->position.top)
                )
            );
        }
        break;

    case ImageScalingMode::Fit:
        {
            float scale = min((this->position.right - this->position.left)/size.width, (this->position.bottom - this->position.top)/size.height);
            this->brush->SetTransform(
                Matrix3x2F::Translation(centerPoint.x - size.width/2.0f, centerPoint.y - size.height/2.0f) *
                Matrix3x2F::Scale(scale, scale, centerPoint)
            );
            this->brushPosition.left = centerPoint.x - scale*size.width/2.0f;
            this->brushPosition.top = centerPoint.y - scale*size.height/2.0f;
            this->brushPosition.right = this->brushPosition.left + scale*size.width;
            this->brushPosition.bottom = this->brushPosition.top + scale*size.height;
        }
        break;

    case ImageScalingMode::Fill:
        {
            float scale = max((this->position.right - this->position.left)/size.width, (this->position.bottom - this->position.top)/size.height);
            this->brush->SetTransform(
                Matrix3x2F::Translation(centerPoint.x - size.width/2.0f, centerPoint.y - size.height/2.0f) *
                Matrix3x2F::Scale(scale, scale, centerPoint)
            );
        }
        break;

    case ImageScalingMode::Tile:
        {
            this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
            reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeX(this->tileModeX);
            reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeY(this->tileModeY);
        }
        break;

    case ImageScalingMode::Edges:
        {
            ComputeEdgeData(size);
        }
        break;
    }
}


void Brush::UpdatePosition(D2D1_RECT_F position)
{
    this->position = this->brushPosition = position;

    if (this->brush)
    {
        if (this->brushType == Type::Image)
        {
            ScaleImage();
        }
        else
        {
            this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
        }
    }
}


bool Brush::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget)
{
    bool ret = false;

    switch (this->brushType)
    {
    case Type::SolidColor:
        {
            if (!this->brushSettings->color->IsConstant() && this->brush)
            {
                ((ID2D1SolidColorBrush*) this->brush)->SetColor(Color::ARGBToD2D(this->brushSettings->color->Evaluate(newColor)));
                ret = true;
            }
        }
        break;

    case Type::LinearGradient:
    case Type::RadialGradient:
        {
            for (UINT i = 0; i < this->gradientStopCount; ++i)
            {
                if (!this->gradientStopColors[i]->IsConstant())
                {
                    this->gradientStops[i].color = Color::ARGBToD2D(this->gradientStopColors[i]->Evaluate(newColor));
                    ret = true;
                }
            }

            if (ret)
            {
                ReCreate(renderTarget);
            }
        }
        break;
    }

    return ret;
}


HRESULT Brush::LoadImageFile(ID2D1RenderTarget *renderTarget, LPCTSTR image, ID2D1Brush **brush)
{
    IWICImagingFactory* factory = NULL;
    IWICBitmap* wicBitmap = NULL;
    IWICFormatConverter* converter = NULL;
    ID2D1Bitmap* bitmap = NULL;
    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
    HRESULT hr = E_FAIL;
        
    HBITMAP hBitmap = LiteStep::LoadLSImage(image, NULL);
    if (hBitmap)
    {
        hr = factory->CreateFormatConverter(&converter);
        if (SUCCEEDED(hr))
        {
            hr = factory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &wicBitmap);
        }
        if (SUCCEEDED(hr))
        {
            hr = converter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
        }
        if (SUCCEEDED(hr))
        {
            hr = renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
        }
        if (SUCCEEDED(hr))
        {
            hr = renderTarget->CreateBitmapBrush(bitmap, reinterpret_cast<ID2D1BitmapBrush**>(brush));
        }

        DeleteObject(hBitmap);
        SAFERELEASE(wicBitmap);
        SAFERELEASE(bitmap);
        SAFERELEASE(converter);
    }

    return hr;
}


void Brush::SetColor(const IColorVal *color)
{
    this->brushSettings->color = std::unique_ptr<IColorVal>(color->Copy());
    if (this->brushType == Type::SolidColor)
    {
        if (this->brush)
        {
            ((ID2D1SolidColorBrush*) this->brush)->SetColor(Color::ARGBToD2D(this->brushSettings->color->Evaluate()));
        }
    }
}


void Brush::SetImage(ID2D1RenderTarget *renderTarget, LPCTSTR path)
{
    this->brushSettings->image = StringUtils::ReallocOverwrite(this->brushSettings->image, path);

    if (this->brushType == Type::Image && renderTarget != nullptr)
    {
        ID2D1Brush *tempBrush;
        if (SUCCEEDED(LoadImageFile(renderTarget, path, &tempBrush)))
        {
            SAFERELEASE(this->brush);
            this->brush = tempBrush;
            this->brush->SetOpacity(this->brushSettings->imageOpacity);
            ScaleImage();
        }
    }
}


BrushSettings *Brush::GetBrushSettings()
{
    return brushSettings;
}
