/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IPainter.hpp
 *  The nModules Project
 *
 *  Any object which can paint.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include <d2d1.h>

class IPainter {
public:
    // 
    virtual void Paint(ID2D1RenderTarget* renderTarget) = 0;

    //
    virtual void DiscardDeviceResources() = 0;

    //
    virtual HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) = 0;

    //
    virtual void UpdatePosition(D2D1_RECT_F parentPosition) = 0;
};
