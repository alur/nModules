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
#include "IStateWindowData.hpp"

template <class StateEnum>
class StateWindowData : public IStateWindowData
{
public:
    StateWindowData()
    {
        ZeroMemory(this, sizeof(*this));
    }

    EnumArray<State::WindowData, StateEnum> data;
    EnumArray<bool, StateEnum> active;
    Window *window;
    StateEnum currentState;
};
