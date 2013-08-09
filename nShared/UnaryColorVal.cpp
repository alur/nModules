/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  UnaryColorVal.cpp
 *  The nModules Project
 *
 *  A color value that is based of another color value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "UnaryColorVal.hpp"


/// <summary>
/// Constructor
/// </summary>
UnaryColorVal::UnaryColorVal(ARGB (*func)(ARGB, long), IColorVal *color, long value)
    : mFunc(func), mColor(color), mValue(value)
{
}


/// <summary>
/// Destructor
/// </summary>
UnaryColorVal::~UnaryColorVal()
{
    delete mColor;
}


/// <summary>
/// IColorVal::IsConstant
/// Returns true if this is a constant value.
/// </summary>
bool UnaryColorVal::IsConstant() const
{
    return false;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB UnaryColorVal::Evaluate() const
{
    return mFunc(mColor->Evaluate(), mValue);
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB UnaryColorVal::Evaluate(ARGB DWMColor) const
{
    return mFunc(mColor->Evaluate(DWMColor), mValue);
}


/// <summary>
/// IColorVal::Evaluate
/// Creates a deep copy of this value.
/// </summary>
IColorVal* UnaryColorVal::Copy() const
{
    return new UnaryColorVal(mFunc, mColor->Copy(), mValue);
}
