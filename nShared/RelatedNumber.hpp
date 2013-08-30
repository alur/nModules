/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelatedNumber.hpp
 *  The nModules Project
 *
 *  Represents a number which is partially dependent on another number.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class RelatedNumber
{
    // Constructor
public:
    RelatedNumber();
    RelatedNumber(float constant);
    RelatedNumber(float constant, float related);

    RelatedNumber operator-(const RelatedNumber&);
    RelatedNumber operator+(const RelatedNumber&);
    RelatedNumber operator*(const float);

    //
public:
    float Evaluate(float related);

    // Member variables
private:
    float mConstantPart;
    float mRelatedPart;
};
