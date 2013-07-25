/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Brush.cpp
*  The nModules Project
*
*  A general brush.
*  
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Brush.hpp"
#include "Color.h"
#include "Factories.h"
#include "LiteStep.h"
#include "Macros.h"
#include <wincodec.h>
#include "../nCoreCom/Core.h"


Brush::Brush() {
    this->brushType = SolidColor;
    this->brush = nullptr;
    this->gradientStops = nullptr;
    this->gradientStopCount = 0;
}


Brush::~Brush() {
    Discard();
    SAFEFREE(this->gradientStops);
}


void Brush::Load(BrushSettings* settings) {
    this->brushSettings = settings;

    if (_stricmp(this->brushSettings->brushType, "Image") == 0) {
        this->brushType = Image;
    }
    else if (_stricmp(this->brushSettings->brushType, "LinearGradient") == 0) {
        this->brushType = LinearGradient;
        this->gradientStart = D2D1::Point2F(this->brushSettings->gradientStartX, this->brushSettings->gradientStartY);
        this->gradientEnd = D2D1::Point2F(this->brushSettings->gradientEndX, this->brushSettings->gradientEndY);
        LoadGradientStops();
    }
    else if (_stricmp(this->brushSettings->brushType, "RadialGradient") == 0) {
        this->brushType = RadialGradient;
        this->gradientCenter = D2D1::Point2F(this->brushSettings->gradientCenterX, this->brushSettings->gradientCenterY);
        this->gradientOriginOffset = D2D1::Point2F(this->brushSettings->gradientOriginOffsetX, this->brushSettings->gradientOriginOffsetY);
        LoadGradientStops();
    }
    else {
        this->brushType = SolidColor;
    }

    if (_stricmp(this->brushSettings->imageScalingMode, "Tile") == 0) {
        this->scalingMode = Tile;
    }
    else if (_stricmp(this->brushSettings->imageScalingMode, "Fit") == 0) {
        this->scalingMode = Fit;
    }
    else if (_stricmp(this->brushSettings->imageScalingMode, "Fill") == 0) {
        this->scalingMode = Fill;
    }
    else if (_stricmp(this->brushSettings->imageScalingMode, "Stretch") == 0) {
        this->scalingMode = Stretch;
    }
    else {
        this->scalingMode = Center;
    }

    if (_stricmp(this->brushSettings->tilingModeX, "Mirror") == 0) {
        this->tileModeX = D2D1_EXTEND_MODE_MIRROR;
    }
    else if (_stricmp(this->brushSettings->tilingModeX, "Clamp") == 0) {
        this->tileModeX = D2D1_EXTEND_MODE_CLAMP;
    }
    else {
        this->tileModeX = D2D1_EXTEND_MODE_WRAP;
    }

    if (_stricmp(this->brushSettings->tilingModeY, "Mirror") == 0) {
        this->tileModeY = D2D1_EXTEND_MODE_MIRROR;
    }
    else if (_stricmp(this->brushSettings->tilingModeY, "Clamp") == 0) {
        this->tileModeY = D2D1_EXTEND_MODE_CLAMP;
    }
    else {
        this->tileModeY = D2D1_EXTEND_MODE_WRAP;
    }
}


void Brush::LoadGradientStops() {
    char colorToken[MAX_LINE_LENGTH], stopToken[MAX_LINE_LENGTH];
    LPCSTR colorPointer = this->brushSettings->gradientColors, stopPointer = this->brushSettings->gradientStops;

    using namespace LiteStep;
    using namespace nCore::InputParsing;

    while (GetToken(colorPointer, colorToken, &colorPointer, FALSE) != FALSE && GetToken(stopPointer, stopToken, &stopPointer, FALSE) != FALSE) {
        ARGB color;
        float stop;
        char* endPtr;

        ParseColor(colorToken, &color);
        stop = (float)strtod(stopToken, &endPtr);

        this->gradientStops = (D2D1_GRADIENT_STOP*)realloc(this->gradientStops, ++this->gradientStopCount*sizeof(D2D1_GRADIENT_STOP));
        this->gradientStops[this->gradientStopCount-1].color = Color::ARGBToD2D(color);
        this->gradientStops[this->gradientStopCount-1].position = stop;
    }
}


//
void Brush::Discard() {
    SAFERELEASE(this->brush);
}


//
HRESULT Brush::ReCreate(ID2D1RenderTarget* renderTarget) {
    HRESULT hr = S_OK;

    if (renderTarget) {
        SAFERELEASE(this->brush);

        switch (this->brushType) {
        case SolidColor:
            {
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings->color), (ID2D1SolidColorBrush**)&this->brush);
            }
            break;

        case LinearGradient:
            {
                ID2D1GradientStopCollection* stops;
                renderTarget->CreateGradientStopCollection(this->gradientStops, this->gradientStopCount, &stops);
                renderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(this->gradientStart, this->gradientEnd),
                    stops, reinterpret_cast<ID2D1LinearGradientBrush**>(&this->brush));
                stops->Release();
            }
            break;

        case RadialGradient:
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

        case Image:
            {
                IWICImagingFactory* factory = NULL;
                IWICBitmap* wicBitmap = NULL;
                IWICFormatConverter* converter = NULL;
                ID2D1Bitmap* bitmap = NULL;
                Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
        
                HBITMAP hBitmap = LiteStep::LoadLSImage(this->brushSettings->image, NULL);
                if (hBitmap) {
                    hr = factory->CreateFormatConverter(&converter);
                    if (SUCCEEDED(hr)) {
                        hr = factory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &wicBitmap);
                    }
                    if (SUCCEEDED(hr)) {
                        hr = converter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
                    }
                    if (SUCCEEDED(hr)) {
                        hr = renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
                    }
                    if (SUCCEEDED(hr)) {
                        hr = renderTarget->CreateBitmapBrush(bitmap, reinterpret_cast<ID2D1BitmapBrush**>(&this->brush));
                    }
                    if (SUCCEEDED(hr)) {
                        this->brush->SetOpacity(this->brushSettings->imageOpacity);
                    }

                    DeleteObject(hBitmap);
                    SAFERELEASE(wicBitmap);
                    SAFERELEASE(bitmap);
                    SAFERELEASE(converter);
                }
                else {
                    hr = E_FAIL;
                }
            }
            break;
        }
    }

    if (SUCCEEDED(hr)) {
        UpdatePosition(this->position);
    }

    return hr;
}


void Brush::ScaleImage() {
    ID2D1Bitmap* bitmap;
    reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->GetBitmap(&bitmap);
    D2D1_SIZE_F size = bitmap->GetSize();
    bitmap->Release();

    using namespace D2D1;

    D2D1_POINT_2F centerPoint = Point2F(
        this->position.left + (this->position.right - this->position.left)/2.0f,
        this->position.top + (this->position.bottom - this->position.top)/2.0f
    );

    switch(this->scalingMode) {
    case Center:
        {
            this->brush->SetTransform(Matrix3x2F::Translation(centerPoint.x - size.width/2.0f, centerPoint.y - size.height/2.0f));
            this->brushPosition.left = max(centerPoint.x - size.width/2.0f, this->position.left);
            this->brushPosition.top = max(centerPoint.y - size.height/2.0f, this->position.top);
            this->brushPosition.right = min(this->brushPosition.left + size.width, this->position.right);
            this->brushPosition.bottom = min(this->brushPosition.top + size.height, this->position.bottom);
        }
        break;

    case Stretch:
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

    case Fit:
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

    case Fill:
        {
            float scale = max((this->position.right - this->position.left)/size.width, (this->position.bottom - this->position.top)/size.height);
            this->brush->SetTransform(
                Matrix3x2F::Translation(centerPoint.x - size.width/2.0f, centerPoint.y - size.height/2.0f) *
                Matrix3x2F::Scale(scale, scale, centerPoint)
            );
        }
        break;

    case Tile:
        {
            this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
            reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeX(this->tileModeX);
            reinterpret_cast<ID2D1BitmapBrush*>(this->brush)->SetExtendModeY(this->tileModeY);
        }
        break;
    }
}


void Brush::UpdatePosition(D2D1_RECT_F position) {
    this->position = this->brushPosition = position;

    if (this->brush) {
        if (this->brushType == Image) {
            ScaleImage();
        }
        else {
            this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
        }
    }
}
