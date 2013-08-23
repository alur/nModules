/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  State.hpp
 *  The nModules Project
 *
 *  Defines a "State" which the window can be in.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "StateSettings.hpp"
#include "Brush.hpp"
#include "IPainter.hpp"
#include "IBrushOwner.hpp"

class State : public IPainter, public IBrushOwner
{
public:
    struct WindowData {

    };

public:
    explicit State(LPCTSTR stateName, Settings *settings, int defaultPriority, LPCWSTR *text);
    virtual ~State();

    void Load(StateSettings* defaultSettings);
    StateSettings* GetSettings();
        
    // Gets the "desired" size for a given width and height.
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size);

    // IPainter
public:
    void DiscardDeviceResources() override;
    void Paint(ID2D1RenderTarget* renderTarget) override;
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) override;
    void UpdatePosition(D2D1_RECT_F position) override;
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override;

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
    bool active;

    // The priority of this state.
    int priority;

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
    StateSettings* drawingSettings;

    // The brush we are currently painting the background with.
    Brush* backBrush;

    // The brush we are currently painting the text with.
    Brush* textBrush;

    // The brush we are currently painting the text with.
    Brush* textShadowBrush;

    // The brush to paint the outline with.
    Brush* outlineBrush;

    // The area we draw text in
    D2D1_RECT_F textArea;

    // Defines how the text is formatted.
    IDWriteTextFormat* textFormat;

    // Points to the windows text.
    LPCWSTR* text;

    //
    LPCSTR stateName;

    // The area we draw in.
    D2D1_ROUNDED_RECT drawingArea;
    
    // The area we draw the outline in.
    D2D1_ROUNDED_RECT outlineArea;

    // The point we rotate text around.
    D2D1_POINT_2F textRotationOrigin;
};
