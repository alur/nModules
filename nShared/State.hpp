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

class State : IPainter {
public:
    explicit State(Settings* settings, int defaultPriority, LPCWSTR* text);
    virtual ~State();

    void Load(StateSettings* defaultSettings);
    void UpdatePosition(D2D1_RECT_F position);
    StateSettings* GetSettings();
        
    // Gets the "desired" size for a given width and height.
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size);

    void DiscardDeviceResources();
    void Paint(ID2D1RenderTarget* renderTarget);
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget);
    void SetTextOffsets(float left, float top, float right, float bottom);

    // The priority of this state.
    int priority;

    // Whether or not this state is currently active.
    bool active;

    // Settings.
    Settings* settings;

private:
    // Creates the text format for this state.
    HRESULT CreateTextFormat(IDWriteTextFormat *&textFormat, bool dropFormat);

    // Creates 
    HRESULT CreateBrush(BrushSettings* settings, ID2D1Brush* brush);

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
    IDWriteTextFormat* textDropFormat;

    // Points to the windows text.
    LPCWSTR* text;

    // The area we draw in.
    D2D1_ROUNDED_RECT drawingArea;

    // The point we rotate text around.
    D2D1_POINT_2F textRotationOrigin;
};
