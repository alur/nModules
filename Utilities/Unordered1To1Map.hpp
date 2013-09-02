/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Unordered1To1Map.hpp
 *  The nModules Project
 *
 *  A hash-based 1-to-1 map
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <assert.h>
#include <forward_list>
#include <initializer_list>
#include <unordered_set>

template<
    class TypeA,
    class TypeB,
    class TypeAHash = std::hash<TypeA>,
    class TypeBHash = std::hash<TypeB>,
    class TypeAEqual = std::equal_to<TypeA>,
    class TypeBEqual = std::equal_to<TypeB>
>
class Unordered1To1Map
{
public:
    struct value_type
    {
        value_type(TypeA a, TypeB b)
            : a(a)
            , b(b)
        {
        }

        TypeA a;
        TypeB b;
    };

private:
    struct listItem
    {
        listItem(value_type value)
            : value(value)
        {
        }
        value_type value;
        typename std::forward_list<listItem>::iterator iter;
    };

    struct acmp
    {
        bool operator()(const TypeA * x, const TypeA * y) const
        {
            return TypeAEqual()(*x, *y);
        }

        size_t operator()(const TypeA * x) const
        {
            return TypeAHash()(*x);
        }
    };

    struct bcmp
    {
        bool operator()(const TypeB * x, const TypeB * y) const
        {
            return TypeBEqual()(*x, *y);
        }

        size_t operator()(const TypeB * x) const
        {
            return TypeBHash()(*x);
        }
    };

private:
    std::forward_list<listItem> mData;
    std::unordered_set<TypeA*, acmp, acmp> mSetA;      // A -> Data
    std::unordered_set<TypeB*, bcmp, bcmp> mSetB;      // B -> Data

public:
    Unordered1To1Map()
    {
    }

    Unordered1To1Map(std::initializer_list<value_type> init)
    {
        for (const value_type & value : init)
        {
            Insert(value);
        }
    }

public:
    value_type * Insert(value_type value)
    {
        //assert(mSetA.find(&value.a) == mSetA.end());
        //assert(mSetB.find(&value.b) == mSetB.end());

        mData.emplace_front(value);
        mData.begin()->iter = mData.begin();
        mSetA.insert(&mData.begin()->value.a);
        mSetB.insert(&mData.begin()->value.b);

        return &mData.begin()->value;
    }

public:
    // Finds a pair based on the 1st value
    value_type * FindByA(TypeA a)
    {
        auto iter = mSetA.find(&a);
        if (iter != mSetA.end())
        {
            return (value_type*)((byte*)*iter - offsetof(value_type, a));
        }

        return nullptr;
    }

    // Finds a pair based on the 2nd value
    value_type * FindByB(TypeB b)
    {
        auto iter = mSetB.find(&b);
        if (iter != mSetB.end())
        {
            return (value_type*)((byte*)*iter - offsetof(value_type, b));
        }

        return nullptr;
    }
};
