/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  BinaryColorVal.cpp
 *  The nModules Project
 *
 *  A color value that is a combination of two other color values.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "BinaryColorVal.hpp"


/// <summary>
/// Constructor
/// </summary>
BinaryColorVal::BinaryColorVal(ARGB (*func)(ARGB, ARGB, float), IColorVal *color1, IColorVal *color2, float value)
    : mFunc(func), mColor1(color1), mColor2(color2), mValue(value)
{
}


/// <summary>
/// Destructor
/// </summary>
BinaryColorVal::~BinaryColorVal()
{
    delete mColor1;
    delete mColor2;
}


/// <summary>
/// IColorVal::IsConstant
/// Returns true if this is a constant value.
/// </summary>
bool BinaryColorVal::IsConstant() const
{
    return false;
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB BinaryColorVal::Evaluate() const
{
    return mFunc(mColor1->Evaluate(), mColor2->Evaluate(), mValue);
}


/// <summary>
/// IColorVal::Evaluate
/// Evaluates this color value.
/// </summary>
ARGB BinaryColorVal::Evaluate(ARGB DWMColor) const
{
    return mFunc(mColor1->Evaluate(DWMColor), mColor2->Evaluate(DWMColor), mValue);
}


/// <summary>
/// IColorVal::Evaluate
/// Creates a deep copy of this value.
/// </summary>
IColorVal* BinaryColorVal::Copy() const
{
    return new BinaryColorVal(mFunc, mColor1->Copy(), mColor2->Copy(), mValue);
}
