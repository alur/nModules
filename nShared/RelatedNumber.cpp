/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelatedNumber.cpp
 *  The nModules Project
 *
 *  Represents a number which is partially dependent on another number.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "RelatedNumber.hpp"


/// <summary>
/// Constructor
/// </summary>
RelatedNumber::RelatedNumber()
{
}


/// <summary>
/// Constructor
/// </summary>
RelatedNumber::RelatedNumber(float constant)
    : RelatedNumber(constant, 0.0f)
{
}


/// <summary>
/// Constructor
/// </summary>
RelatedNumber::RelatedNumber(float constant, float related)
    : mConstantPart(constant)
    , mRelatedPart(related)
{
}


/// <summary>
/// Subtraction operator
/// </summary>
RelatedNumber RelatedNumber::operator-(const RelatedNumber &other)
{
    return RelatedNumber(this->mConstantPart - other.mConstantPart, this->mRelatedPart - other.mRelatedPart);
}


/// <summary>
/// Addition operator
/// </summary>
RelatedNumber RelatedNumber::operator+(const RelatedNumber &other)
{
    return RelatedNumber(this->mConstantPart + other.mConstantPart, this->mRelatedPart + other.mRelatedPart);
}


/// <summary>
/// Multiplication operator
/// </summary>
RelatedNumber RelatedNumber::operator*(const float factor)
{
    return RelatedNumber(this->mConstantPart * factor, this->mRelatedPart * factor);
}


/// <summary>
/// Evaluates the related number, given the specified value
/// </summary>
float RelatedNumber::Evaluate(float related)
{
    return related * mRelatedPart + mConstantPart;
}
