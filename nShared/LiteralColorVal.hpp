/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LiteralColorVal.hpp
 *  The nModules Project
 *
 *  A literal color value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IColorVal.hpp"

class LiteralColorVal : public IColorVal
{
public:
    explicit LiteralColorVal(ARGB value);

public:
    bool IsConstant() const override;
    ARGB Evaluate() const override;
    ARGB Evaluate(ARGB DWMColor) const override;
    IColorVal* Copy() const override;

private:
    ARGB mValue;
};
