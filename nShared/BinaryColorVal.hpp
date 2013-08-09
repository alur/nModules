/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  BinaryColorVal.hpp
 *  The nModules Project
 *
 *  A color value that is a combination of two other color values.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IColorVal.hpp"

class BinaryColorVal : public IColorVal
{
public:
    explicit BinaryColorVal(ARGB (*func)(ARGB, ARGB, float), IColorVal*, IColorVal*, float);
    virtual ~BinaryColorVal();

public:
    bool IsConstant() const override;
    ARGB Evaluate() const override;
    ARGB Evaluate(ARGB DWMColor) const override;
    IColorVal* Copy() const override;

private:
    ARGB (*mFunc)(ARGB, ARGB, float);
    IColorVal *mColor1;
    IColorVal *mColor2;
    float mValue;
};
