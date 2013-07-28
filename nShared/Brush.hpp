/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Brush.hpp
 *  The nModules Project
 *
 *  A general brush.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "BrushSettings.hpp"

class Brush {
public:
    explicit Brush();
    virtual ~Brush();

    // Loads brush settings.
    void Load(BrushSettings *settings);

    // Updates the brush to the new position of the window.
    void UpdatePosition(D2D1_RECT_F position);

    // The brush.
    ID2D1Brush *brush;

    //
    D2D1_RECT_F brushPosition;

    // Discards the brush.
    void Discard();

    // Recreates the brush.
    HRESULT ReCreate(ID2D1RenderTarget *renderTarget);

    HRESULT LoadImageFile(ID2D1RenderTarget *renderTarget, LPCSTR image, ID2D1Brush **brush);

public:
    void SetColor(ARGB color); 
    void SetImage(ID2D1RenderTarget *renderTarget, LPCSTR path); 

private:
    // The type of brush this is.
    enum BrushType {
        SolidColor,
        LinearGradient,
        RadialGradient,
        Image
    } brushType;

    // How to scale the brush.
    enum ImageScalingMode {
        Fit,
        Fill,
        Stretch,
        Center,
        Tile
    } scalingMode;

    // Loads render targets.
    void LoadGradientStops();

    //
    void ScaleImage();
    
    // The current drawing settings.
    BrushSettings* brushSettings;

    //
    D2D1_EXTEND_MODE tileModeX;

    //
    D2D1_EXTEND_MODE tileModeY;

    // The current position of the 
    D2D1_RECT_F position;

    // The gradient stops.
    D2D1_GRADIENT_STOP* gradientStops;

    // The number of gradient stops we have.
    UINT gradientStopCount;

    union {
        // The center of a radial gradient.
        D2D1_POINT_2F gradientCenter;

        // The starting point of a linear gradient.
        D2D1_POINT_2F gradientStart;
    };
    
    union {
        // How far away from the center the original of the radial gradient is.
        D2D1_POINT_2F gradientOriginOffset;

        // The ending point of a linear gradient.
        D2D1_POINT_2F gradientEnd;
    };
};
