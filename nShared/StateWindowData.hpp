/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateWindowData.hpp
 *  The nModules Project
 *
 *  Holds the data required to paint a particular state for a particular
 *  window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/EnumArray.hpp"
#include "State.hpp"

template <class StateEnum>
class StateWindowData : public StateWindowData
{
public:
    EnumArray<WindowData, StateEnum> data;
};
