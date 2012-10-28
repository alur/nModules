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
    this->brush = NULL;
    this->gradientStops = NULL;
    this->gradientStopCount = 0;
}


Brush::~Brush() {
    Discard();
}


void Brush::Load(BrushSettings* settings) {
    this->brushSettings = *settings;

    if (_stricmp(this->brushSettings.brushType, "Image") == 0) {
        this->brushType = Image;
    }
    else if (_stricmp(this->brushSettings.brushType, "LinearGradient") == 0) {
        this->brushType = LinearGradient;
        this->gradientStart = D2D1::Point2F(this->brushSettings.gradientStartX, this->brushSettings.gradientStartY);
        this->gradientEnd = D2D1::Point2F(this->brushSettings.gradientEndX, this->brushSettings.gradientEndY);
        LoadGradientStops();
    }
    else if (_stricmp(this->brushSettings.brushType, "RadialGradient") == 0) {
        this->brushType = RadialGradient;
        this->gradientCenter = D2D1::Point2F(this->brushSettings.gradientCenterX, this->brushSettings.gradientCenterY);
        this->gradientOriginOffset = D2D1::Point2F(this->brushSettings.gradientOriginOffsetX, this->brushSettings.gradientOriginOffsetY);
        LoadGradientStops();
    }
    else {
        this->brushType = SolidColor;
    }
}


void Brush::LoadGradientStops() {
    char colorToken[MAX_LINE_LENGTH], stopToken[MAX_LINE_LENGTH];
    LPCSTR colorPointer = this->brushSettings.gradientColors, stopPointer = this->brushSettings.gradientStops;

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


void Brush::UpdatePosition(D2D1_RECT_F position) {
    this->position = position;

    if (this->brush) {
        this->brush->SetTransform(D2D1::Matrix3x2F::Identity());
        this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
    }
}


//
void Brush::Discard() {
    SAFERELEASE(this->brush);
    SAFEFREE(this->gradientStops);
}


//
HRESULT Brush::ReCreate(ID2D1RenderTarget* renderTarget) {
    HRESULT hr = S_OK;

    if (renderTarget) {
        SAFERELEASE(this->brush);

        switch (this->brushType) {
        case SolidColor:
            {
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings.color), (ID2D1SolidColorBrush**)&this->brush);
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
                    this->brushSettings.gradientRadiusX, this->brushSettings.gradientRadiusY),
                    stops, reinterpret_cast<ID2D1RadialGradientBrush**>(&this->brush));
                stops->Release();
            }
            break;

        case Image:
            {
                if (this->brushSettings.image[0] != 0) {
                    IWICImagingFactory* factory = NULL;
                    IWICBitmap* wicBitmap = NULL;
                    IWICFormatConverter* converter = NULL;
                    ID2D1Bitmap* bitmap = NULL;
                    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
        
                    HBITMAP hBitmap = LiteStep::LoadLSImage(this->brushSettings.image, NULL);
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
                            this->brush->SetOpacity(this->brushSettings.imageOpacity);
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
            }
            break;
        }
    }

    if (SUCCEEDED(hr)) {
        this->brush->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
    }

    return hr;
}
