/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SelectionRectangle.hpp
 *  The nModules Project
 *
 *  Draws the selection rectangle, as a post-processing step.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/IPainter.hpp"
#include "../nShared/Brush.hpp"

class SelectionRectangle : public IPainter {
    // Constructor/Destructor
public:
    explicit SelectionRectangle();
    virtual ~SelectionRectangle();

    // IPainter
public:
    void Paint(ID2D1RenderTarget *renderTarget) override;
    void DiscardDeviceResources() override;
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) override;

    //
public:
    void Init(Settings *parentSettings);
    void SetRect(D2D1_RECT_F rect);
    void Show();
    void Hide();

    //
private:
    Brush mBackBrush;
    BrushSettings mBackBrushSettings;

    Brush mOutlineBrush;
    BrushSettings mOutlineBrushSettings;

    float mOutlineWidth;
    D2D1_ROUNDED_RECT mRect;
    bool mHidden;
};
