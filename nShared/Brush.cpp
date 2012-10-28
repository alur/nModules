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


Brush::Brush() {
    this->brushType = SolidColor;
    this->brush = NULL;
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
    }
    else if (_stricmp(this->brushSettings.brushType, "RadialGradient") == 0) {
        this->brushType = RadialGradient;
    }
    else {
        this->brushType = SolidColor;
    }
}


void Brush::UpdatePosition(D2D1_RECT_F position) {
    this->position = position;

    if (this->brushType == Image && this->brush) {
        (reinterpret_cast<ID2D1BitmapBrush*>(this->brush))->SetTransform(D2D1::Matrix3x2F::Identity());
        (reinterpret_cast<ID2D1BitmapBrush*>(this->brush))->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
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
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings.color), (ID2D1SolidColorBrush**)&this->brush);
            }
            break;

        case LinearGradient:
            {
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings.color), (ID2D1SolidColorBrush**)&this->brush);
            }
            break;

        case RadialGradient:
            {
                renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->brushSettings.color), (ID2D1SolidColorBrush**)&this->brush);
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
                            (reinterpret_cast<ID2D1BitmapBrush*>(this->brush))->SetTransform(D2D1::Matrix3x2F::Translation(this->position.left, this->position.top));
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

    return hr;
}
