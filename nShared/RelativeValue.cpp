/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelativeValue.cpp
 *  The nModules Project
 *
 *  A value which is partially defined by another value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "RelativeValue.hpp"


RelativeValue::RelativeValue(float relative, float absolute) {
    this->relative = relative;
    this->absolute = absolute;
}


float RelativeValue::GetValue(float source) {
    return source*this->relative+this->absolute;
}
