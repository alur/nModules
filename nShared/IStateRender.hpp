/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IStateRender.hpp
 *  The nModules Project
 *
 *  Interface to a StateRender object, of any type.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IStateWindowData.hpp"

class IStateRender
{
public:
    virtual void SetTextOffsets(float left, float top, float right, float bottom) = 0;
    virtual void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, class Window *window) = 0;
    virtual class State *GetState(LPCTSTR name) = 0;
    virtual IStateWindowData *CreateWindowData(class Window *window) = 0;
    virtual void Paint(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) = 0;
    virtual void PaintText(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) = 0;
    virtual void UpdatePosition(D2D1_RECT_F parentPosition, IStateWindowData *windowData) = 0;
    virtual void DiscardDeviceResources() = 0;
    virtual HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) = 0;
    virtual bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) = 0;
    virtual void UpdateText(IStateWindowData *windowData) = 0;
};
