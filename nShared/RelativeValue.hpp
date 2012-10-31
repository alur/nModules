/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelativeValue.hpp
 *  The nModules Project
 *
 *  A value which is partially defined by another value.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class RelativeValue {
public:
    static RelativeValue Parse();

    explicit RelativeValue(float relative, float absolute);

    float GetValue(float source);

private:
    float relative;
    float absolute;
};
