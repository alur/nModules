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

    struct WindowData
    {
        WindowData()
        {
            brushTransform = D2D1::Matrix3x2F::Identity();
        }

        // The current position of the window we are drawing to.
        D2D1_RECT_F position;

        // The position, within the window, which we should actually draw to. (for center scaling and such).
        D2D1_RECT_F brushPosition;
        
        // The current transform of the brush
        D2D1_MATRIX_3X2_F brushTransform;

        // If we are in Edges mode, the rectangles of the various partitions.
        EnumArray<D2D1_RECT_F, EdgeType> imageEdgeRects;

        // If we are in Edges scaling mode, the transforms used for the various edges.
        EnumArray<D2D1_MATRIX_3X2_F, EdgeType> imageEdgeTransforms;

        // The last the time the brush position and transforms was computed.
        ULONGLONG transformComputationTime;
    };

public:
    explicit Brush();
    virtual ~Brush();

public:
    // Loads brush settings.
    void Load(BrushSettings *settings);

    // Updates the brush to the new position of the window.
    void UpdatePosition(D2D1_RECT_F position, WindowData *windowData);

    //
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget);

    // The brush.
    ID2D1Brush *brush;

    // Discards the brush.
    void Discard();

    // Recreates the brush.
    HRESULT ReCreate(ID2D1RenderTarget *renderTarget);

    HRESULT LoadImageFile(ID2D1RenderTarget *renderTarget, LPCTSTR image, ID2D1Brush **brush);

private:
    void UpdateTransform(WindowData *windowData);

public:
    bool IsImageEdgeBrush() const;
    D2D1_RECT_F *GetImageEdgeRectAndScaleBrush(EdgeType edgeType, WindowData *windowData);
    void ComputeEdgeData(D2D1_SIZE_F size, WindowData *windowData);

public:
    void SetColor(const IColorVal *color); 
    void SetImage(ID2D1RenderTarget *renderTarget, LPCTSTR path); 
    BrushSettings *GetBrushSettings();
    void CheckTransforms(WindowData *wndData);

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
    void ScaleImage(WindowData *windowData);
    
    // The current drawing settings.
    BrushSettings* brushSettings;

    //
    D2D1_EXTEND_MODE tileModeX;

    //
    D2D1_EXTEND_MODE tileModeY;

    // Defines how to scale the image for the Edges scaling mode.
    D2D1_RECT_F imageEdges;

    // The gradient stops.
    D2D1_GRADIENT_STOP* gradientStops;
    
    //
    IColorVal** gradientStopColors;

    // The number of gradient stops we have.
    UINT gradientStopCount;

    // The last time a change which requires the transforms to be recomputed occured.
    ULONGLONG mTransformTimeStamp;

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
