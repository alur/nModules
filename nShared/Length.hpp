/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Length.hpp
 *  The nModules Project
 *
 *  Represents a length.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class Length
{
    // Constructor
public:
    Length(float constant, float percentage);

public:

    // Member variables
private:
    float mConstantPart;
    float mPercentagePart;
};
