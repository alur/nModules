/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Coordinate.hpp
 *  The nModules Project
 *
 *  Represents a coordinate, in one dimension.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class Coordinate
{
    // Constructor
public:
    Coordinate(float constant, float percentage);

public:

    // Member variables
private:
    float mConstantPart;
    float mPercentagePart;
};
