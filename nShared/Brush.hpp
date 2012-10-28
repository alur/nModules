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

    void Load(BrushSettings* settings);
    void UpdatePosition(D2D1_RECT_F position);

    // The brush.
    ID2D1Brush* brush;

    //
    void Discard();

    //
    HRESULT ReCreate(ID2D1RenderTarget* renderTarget);

private:
    enum BrushType {
        SolidColor,
        LinearGradient,
        RadialGradient,
        Image
    };
    
    // The current drawing settings.
    BrushSettings brushSettings;

    //
    BrushType brushType;

    //
    D2D1_RECT_F position;
};
