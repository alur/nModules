/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  State.hpp
 *  The nModules Project
 *
 *  Defines a "State" which the window can be in.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class State;

#include "StateSettings.hpp"
#include "Brush.hpp"
#include "IPainter.hpp"
#include "IBrushOwner.hpp"
#include "Window.hpp"

class State : public IBrushOwner
{
public:
    struct WindowData
    {
        // The area we draw text in
        D2D1_RECT_F textArea;

        // The area we draw in.
        D2D1_ROUNDED_RECT drawingArea;
    
        // The area we draw the outline in.
        D2D1_ROUNDED_RECT outlineArea;

        // The point we rotate text around.
        D2D1_POINT_2F textRotationOrigin;
    };

public:
    explicit State();
    virtual ~State();

    void Load(StateSettings* defaultSettings, LPCTSTR prefix, Settings *settings);
    StateSettings* GetSettings();
        
    // Gets the "desired" size for a given width and height.
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, class Window *window);

    // IPainter
public:
    void DiscardDeviceResources();
    void Paint(ID2D1RenderTarget* renderTarget, WindowData *windowData, class Window *window);
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget);
    void UpdatePosition(D2D1_RECT_F position, WindowData *windowData);
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget);

    // IBrushOwner
public:
    Brush *GetBrush(LPCTSTR name) override;

public:
    void SetCornerRadiusX(float radius);
    void SetCornerRadiusY(float radius);
    void SetOutlineWidth(float width);

    void SetTextOffsets(float left, float top, float right, float bottom);

    void SetReadingDirection(DWRITE_READING_DIRECTION direction);
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
    void SetTextRotation(float rotation);
    void SetTextTrimmingGranuality(DWRITE_TRIMMING_GRANULARITY granularity);
    void SetTextVerticalAlign(DWRITE_PARAGRAPH_ALIGNMENT alignment);
    void SetWordWrapping(DWRITE_WORD_WRAPPING wrapping);

public:
    // The name of this state.
    LPCTSTR mName;

    // Settings.
    Settings* settings;

private:
    // Creates the text format for this state.
    HRESULT CreateTextFormat(IDWriteTextFormat *&textFormat);

    // Creates 
    HRESULT CreateBrush(BrushSettings* settings, ID2D1Brush* brush);

private:
    // The current drawing settings.
    StateSettings mStateSettings;

    // The brush we are currently painting the background with.
    Brush mBackBrush;

    // The brush we are currently painting the text with.
    Brush mTextBrush;

    // The brush we are currently painting the text with.
    Brush mTextShadowBrush;

    // The brush to paint the outline with.
    Brush mOutlineBrush;

    // Defines how the text is formatted.
    IDWriteTextFormat* textFormat;

    //
    LPCSTR stateName;
};
