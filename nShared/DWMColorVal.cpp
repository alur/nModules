/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DWMColorVal.cpp
 *  The nModules Project
 *
 *  Represents the DWM color.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "DWMColorVal.hpp"
#include <dwmapi.h>


/// <summary>
/// Constructor
/// </summary>
DWMColorVal::DWMColorVal()
{
}


/// <summary>
/// IColorVal::IsConstant
/// Returns true if this is a constant value.
/// </summary>
bool DWMColorVal::IsConstant() const
{
    return false;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB DWMColorVal::Evaluate() const
{
    ARGB color;
    BOOL opaque;
    DwmGetColorizationColor(&color, &opaque);

    // When the intensity is really high, the alpha drops to 0 :/
    if (color >> 24 == 0 && color != 0)
    {
        color |= 0xFF000000;
    }

    return color;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB DWMColorVal::Evaluate(ARGB DWMColor) const
{
    return DWMColor;
}


/// <summary>
/// IColorVal::Evaluate
/// Creates a deep copy of this value.
/// </summary>
IColorVal* DWMColorVal::Copy() const
{
    return new DWMColorVal();
}
