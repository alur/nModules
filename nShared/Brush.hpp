/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Brush.hpp
 *  The nModules Project
 *
 *  A general brush.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "BrushSettings.hpp"
#include "../Utilities/EnumArray.hpp"

class Brush {
public:
    enum class EdgeType
    {
        TopLeft = 0,
        TopCenter,
        TopRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
        Count
    };

public:
    explicit Brush();
    virtual ~Brush();

public:
    // Loads brush settings.
    void Load(BrushSettings *settings);

    // Updates the brush to the new position of the window.
    void UpdatePosition(D2D1_RECT_F position);

    //
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget);

    // The brush.
    ID2D1Brush *brush;

    //
    D2D1_RECT_F brushPosition;

    // Discards the brush.
    void Discard();

    // Recreates the brush.
    HRESULT ReCreate(ID2D1RenderTarget *renderTarget);

    HRESULT LoadImageFile(ID2D1RenderTarget *renderTarget, LPCTSTR image, ID2D1Brush **brush);

public:
    bool IsImageEdgeBrush();
    D2D1_RECT_F *GetImageEdgeRectAndScaleBrush(EdgeType edgeType);
    void ComputeEdgeData(D2D1_SIZE_F size);

public:
    void SetColor(const IColorVal *color); 
    void SetImage(ID2D1RenderTarget *renderTarget, LPCTSTR path); 
    BrushSettings *GetBrushSettings();

private:
    // The type of brush this is.
    enum class Type
    {
        SolidColor,
        LinearGradient,
        RadialGradient,
        Image
    } brushType;

    // How to scale the brush.
    enum class ImageScalingMode
    {
        Fit,
        Fill,
        Stretch,
        Center,
        Tile,
        Edges
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

    // Defines how to scale the image for the Edges scaling mode.
    D2D1_RECT_F imageEdges;

    //
    EnumArray<D2D1_RECT_F, EdgeType> mImageEdgeRects;

    //
    EnumArray<D2D1_MATRIX_3X2_F, EdgeType> mImageEdgeTransforms;

    // The gradient stops.
    D2D1_GRADIENT_STOP* gradientStops;
    
    //
    IColorVal** gradientStopColors;

    // The number of gradient stops we have.
    UINT gradientStopCount;

    union
    {
        // The center of a radial gradient.
        D2D1_POINT_2F gradientCenter;

        // The starting point of a linear gradient.
        D2D1_POINT_2F gradientStart;
    };
    
    union
    {
        // How far away from the center the original of the radial gradient is.
        D2D1_POINT_2F gradientOriginOffset;

        // The ending point of a linear gradient.
        D2D1_POINT_2F gradientEnd;
    };
};
