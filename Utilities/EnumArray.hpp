//--------------------------------------------------------------------------------------
// EnumArray.hpp
// The nModules Project
//
// Lets you easily create and manage an array based on a class enum.
//
// There are three requirements:
// 1. The first element in the enum class must be = 0.
// 2. No other values may be specified.
// 3. The last element of the enum must be Count.
//
//--------------------------------------------------------------------------------------
#pragma once

template<class ElementType, class IndexType>
class EnumArray {
public:
    /// <summary>
    /// Constructor
    /// </summary>
    explicit EnumArray() {
    }
    
    /// <summary>
    /// Constructor
    /// </summary>
    explicit EnumArray(ElementType start, ...) {
        va_list list;
        va_start(list, start);

        for (ElementType &element : mArray) {
            element = va_arg(list, ElementType);
        }

        va_end(list);
    }

public:
    /// <summary>
    /// Retrives the element correspoding to the specified index.
    /// </summary>
    ElementType &operator[] (IndexType index) {
        return mArray[std::underlying_type<IndexType>::type(index)];
    }
    

    /// <summary>
    /// Returns a pointer to the first element in the array.
    /// </summary>
    ElementType *begin() {
        return &mArray[0];
    }
    

    /// <summary>
    /// Returns a pointer to the one-past-last element in the array.
    /// </summary>
    ElementType *end() {
        return &mArray[std::underlying_type<IndexType>::type(IndexType::Count)];
    }


    /// <summary>
    /// Sets every element in the array to some value.
    /// </summary>
    void SetAll(ElementType value) {
        for (ElementType &element : mArray) {
            element = value;
        }
    }

private:
    /// <summary>
    /// Contains the actual array elements.
    /// </summary>
    ElementType mArray[IndexType::Count];
};
