/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Overlay.hpp
 *  The nModules Project
 *
 *  Defines an overlay. A bitmap, or icon, or something.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IPainter.hpp"

class Overlay : IPainter {
public:
    explicit Overlay(D2D1_RECT_F position, D2D1_RECT_F parentPosition, IWICBitmapSource* source, int zOrder);
    virtual ~Overlay();

    // IPainter
public:
    void DiscardDeviceResources() override;
    void Paint(ID2D1RenderTarget* renderTarget) override;
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) override;
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override;
    void UpdatePosition(D2D1_RECT_F parentPosition);

    void SetSource(IWICBitmapSource* source);
    ID2D1BitmapBrush *GetBrush();
    int GetZOrder();

private:
    D2D1_RECT_F position;
    D2D1_RECT_F drawingPosition;
    ID2D1BitmapBrush* brush;
    IWICBitmapSource* source; // We need to keep the source image in order to be able to recreate the overlay.
    ID2D1RenderTarget* renderTarget;
    int zOrder;
};
