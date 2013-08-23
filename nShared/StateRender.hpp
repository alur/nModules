/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateRender.hpp
 *  The nModules Project
 *
 *  Manages states, renders the window background.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IStateRender.hpp"
#include "State.hpp"
#include "../Utilities/EnumArray.hpp"
#include <list>

template <class StateEnum>
class StateRender : public IStateRender
{
public:
    EnumArray<State, StateEnum> mStates;
    std::list<State*> mStateOrder;

public:
    StateRender()
    {
    }

public:
    /// <summary>
    /// Activates the specified state.
    /// </summary>
    void ActivateState(StateEnum state)
    {

    }
    

    /// <summary>
    /// Clears the specified state.
    /// </summary>
    void ClearState(StateEnum state)
    {
         
    }
    

    /// <summary>
    /// Toggles the specified state.
    /// </summary>
    void ToggleState(StateEnum state)
    {
        if (mStates[state].active)
        {
            ClearState(state);
        }
        else
        {
            ActivateState(state);
        }
    }

    
    IStateWindowData *CreateWindowData() override
    {

    }

    
    /// <summary>
    /// Paints the currently active state to the specified render target, using
    /// the specified window data.
    /// </summary>
    void Paint(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) const override
    {

    }

    
    /// <summary>
    /// Updates the window data based on the given window position.
    /// </summary>
    void UpdatePosition(D2D1_RECT_F parentPosition, IStateWindowData *windowData) const override
    {

    }


    void DiscardDeviceResources() override
    {

    }


    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) override
    {
        for (State &state : mStates)
        {

        }
    }


    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override
    {
        bool ret = false;

        for (State &state : mStates)
        {
            ret = state.UpdateDWMColor(newColor, this->renderTarget) || ret;
        }

        return ret;
    }
};
