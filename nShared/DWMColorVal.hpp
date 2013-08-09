/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DWMColorVal.hpp
 *  The nModules Project
 *
 *  Represents the DWM color.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IColorVal.hpp"

class DWMColorVal : public IColorVal
{
public:
    explicit DWMColorVal();

public:
    bool IsConstant() const override;
    ARGB Evaluate() const override;
    ARGB Evaluate(ARGB DWMColor) const override;
    IColorVal* Copy() const override;
};
