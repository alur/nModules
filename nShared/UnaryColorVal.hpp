/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  UnaryColorVal.hpp
 *  The nModules Project
 *
 *  A color value that is based of another color value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IColorVal.hpp"

class UnaryColorVal : public IColorVal
{
public:
    explicit UnaryColorVal(ARGB (*func)(ARGB, long), IColorVal*, long);
    ~UnaryColorVal();

public:
    bool IsConstant() const override;
    ARGB Evaluate() const override;
    ARGB Evaluate(ARGB DWMColor) const override;
    IColorVal* Copy() const override;

private:
    ARGB (*mFunc)(ARGB, long);
    IColorVal *mColor;
    long mValue;
};
