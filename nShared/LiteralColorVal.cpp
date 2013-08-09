/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LiteralColorVal.cpp
 *  The nModules Project
 *
 *  A literal color value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteralColorVal.hpp"


/// <summary>
/// Constructor
/// </summary>
LiteralColorVal::LiteralColorVal(ARGB value)
    : mValue(value)
{
}


/// <summary>
/// IColorVal::IsConstant
/// Returns true if this is a constant value.
/// </summary>
bool LiteralColorVal::IsConstant() const
{
    return true;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB LiteralColorVal::Evaluate() const
{
    return mValue;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB LiteralColorVal::Evaluate(ARGB /* DWMColor */) const
{
    return mValue;
}


/// <summary>
/// IColorVal::Evaluate
/// Creates a deep copy of this value.
/// </summary>
IColorVal* LiteralColorVal::Copy() const
{
    return new LiteralColorVal(mValue);
}
