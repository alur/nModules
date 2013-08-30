/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelatedNumber.hpp
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
    : RelatedNumber(0.0f, 0.0f)
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
/// Evaluates the related number, given the specified value
/// </summary>
float RelatedNumber::Evaluate(float related)
{
    return related * mRelatedPart + mConstantPart;
}
