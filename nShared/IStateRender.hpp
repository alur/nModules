/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IStateRender.hpp
 *  The nModules Project
 *
 *  Interface to a StateRender object, of any type.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IPainter.hpp"
#include "IStateWindowData.hpp"

class IStateRender : public IPainter
{
public:
    virtual IStateWindowData *CreateWindowData() = 0;
    virtual void Paint(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) const = 0;
    virtual void UpdatePosition(D2D1_RECT_F parentPosition, IStateWindowData *windowData) const = 0;
    virtual void DiscardDeviceResources() = 0;
    virtual HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) = 0;
    virtual bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) = 0;
};
