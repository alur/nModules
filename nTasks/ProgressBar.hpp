/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ProgressBar.hpp
 *  The nModules Project
 *
 *  Draws the progress bar on a task button.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class ProgressBar : public IPainter
{
public:
    // Contains all the brushes needed to draw the progress bar.
    // These are shared between all progressbars belonging to a particular taskbar.
    class Shared
    {
    public:
        Brush mBackBrush;
        Brush mNormalBarBrush;
    };

    // Constructor/Destructor
public:
    explicit ProgressBar();
    virtual ~ProgressBar();

    // IPainter
public:
    void Paint(ID2D1RenderTarget *renderTarget) override;
    void DiscardDeviceResources() override;
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
    void UpdatePosition(D2D1_RECT_F parentPosition) override;
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override;

private:
    Brush::WindowData mBackWindowData;
    Brush::WindowData mNormalBarWindowData;
};
