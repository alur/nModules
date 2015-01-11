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
#include "StateWindowData.hpp"
#include "../Utilities/EnumArray.hpp"
#include <list>

template <class StateEnum>
class StateRender : public IStateRender
{
public:
    EnumArray<State, StateEnum> mStates;

    struct StateInitData
    {
        StateInitData()
        {
            priority = 0;
            prefix = L"";
            base = StateEnum::Base;
        }

        std::list<StateEnum> dependencies;
        State::Settings defaults;
        int priority;
        LPCTSTR prefix;
        StateEnum base;
    };

    typedef EnumArray<StateInitData, StateEnum> InitData;

private:
    int mDeviceRefCount;
    EnumArray<std::list<StateEnum>, StateEnum> mDependentStates;
    EnumArray<std::list<StateEnum>, StateEnum> mStateDependencies;

public:
    /// <summary>
    /// Constructor.
    /// </summary>
    StateRender()
    {
        mDeviceRefCount = 0;
    }

public:
    /// <summary>
    /// Activates the specified state.
    /// </summary>
    void ActivateState(StateEnum state, Window *window)
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(window->GetWindowData());

        if (!data->active[state])
        {
            data->active[state] = true;

            for (StateEnum state : mDependentStates[state])
            {
                bool shouldActivate = true;
                for (StateEnum state : mStateDependencies[state])
                {
                    if (!data->active[state])
                    {
                        shouldActivate = false;
                        break;
                    }
                }

                if (shouldActivate)
                {
                    ActivateState(state, window);
                }
            }

            if (data->currentState < state)
            {
                data->currentState = state;
                window->Repaint();
            }
        }
    }
    

    /// <summary>
    /// Clears the specified state.
    /// </summary>
    void ClearState(StateEnum state, Window *window)
    {
        ASSERT(state != StateEnum::Base);

        StateWindowData<StateEnum> *data;
        data = decltype(data)(window->GetWindowData());

        if (data->active[state])
        {
            data->active[state] = false;

            if (state == data->currentState)
            {
                for (; !data->active[data->currentState] && data->currentState != StateEnum::Base; EnumDecrement(data->currentState));
                window->Repaint();
            }
            
            for (StateEnum state : mDependentStates[state])
            {
                ClearState(state, window);
            }
        }
    }
    

    /// <summary>
    /// Toggles the specified state.
    /// </summary>
    void ToggleState(StateEnum state, Window *window)
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(window->GetWindowData());
        if (data->active[state])
        {
            ClearState(state, window);
        }
        else
        {
            ActivateState(state, window);
        }
    }
    

    /// <summary>
    /// Toggles the specified state.
    /// </summary>
    bool IsStateActive(StateEnum state, Window *window)
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(window->GetWindowData());
        return data->active[state];
    }

    
    /// <summary>
    /// 
    /// </summary>
    State *GetState(LPCTSTR name)
    {
        for (State &state : mStates)
        {
          if (_wcsicmp(name, state.mName) == 0)
            {
                return &state;
            }
        }
        return nullptr;
    }


    void Load(Settings *baseSettings) {
      const InitData initData;
      Load(initData, baseSettings);
    }
    

    void Load(const InitData & initData, Settings *baseSettings)
    {
        for (StateEnum state = StateEnum::Base; state != StateEnum::Count; EnumIncrement(state))
        {
            Settings *stateSettings = baseSettings->CreateChild(initData[state].prefix);
            if (state != StateEnum::Base)
            {
                stateSettings->AppendGroup(mStates[initData[state].base].settings);
            }
            mStateDependencies[state] = initData[state].dependencies;
            for (StateEnum depState : mStateDependencies[state])
            {
                mDependentStates[depState].push_back(state);
            }
            mStates[state].Load(&initData[state].defaults, stateSettings, initData[state].prefix);
        }
    }

    
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, Window *window) override
    {
        mStates[StateEnum::Base].GetDesiredSize(maxWidth, maxHeight, size, window);
    }


    void SetTextOffsets(float left, float top, float right, float bottom) override
    {
        for (State &state : mStates)
        {
            state.SetTextOffsets(left, top, right, bottom);
        }
    }


    /// <summary>
    /// 
    /// </summary>
    IStateWindowData *CreateWindowData(Window * window) override
    {
        StateWindowData<StateEnum> *data = new StateWindowData<StateEnum>();
        data->window = window;
        return data;
    }

    
    /// <summary>
    /// Paints the currently active state to the specified render target, using
    /// the specified window data.
    /// </summary>
    void Paint(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) override
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(windowData);

        mStates[data->currentState].Paint(renderTarget, &data->data[data->currentState]);
    }

    
    /// <summary>
    /// Paints the currently active state to the specified render target, using
    /// the specified window data.
    /// </summary>
    void PaintText(ID2D1RenderTarget* renderTarget, IStateWindowData *windowData) override
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(windowData);

        mStates[data->currentState].PaintText(renderTarget, &data->data[data->currentState], data->window);
    }

    
    /// <summary>
    /// Updates the window data based on the given window position.
    /// </summary>
    void UpdatePosition(D2D1_RECT_F parentPosition, IStateWindowData *windowData) override
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(windowData);
        
        for (StateEnum state = StateEnum::Base; state != StateEnum::Count; EnumIncrement(state))
        {
            mStates[state].UpdatePosition(parentPosition, &data->data[state]);
        }
    }


    void DiscardDeviceResources() override
    {
        if (--mDeviceRefCount == 0)
        {
            for (State &state : mStates)
            {
                state.DiscardDeviceResources();
            }
        }
    }


    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) override
    {
        ASSERT(renderTarget != nullptr);
        HRESULT hr = S_OK;

        if (mDeviceRefCount == 0)
        {
            for (State &state : mStates)
            {
                RETURNONFAIL(hr, state.ReCreateDeviceResources(renderTarget));
            }
        }
        
        // The resources are already created, or we succeeded.
        ++mDeviceRefCount;

        return hr;
    }


    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override
    {
        bool ret = false;

        for (State &state : mStates)
        {
            ret = state.UpdateDWMColor(newColor, renderTarget) || ret;
        }

        return ret;
    }

    void UpdateText(IStateWindowData *windowData) override
    {
        StateWindowData<StateEnum> *data;
        data = decltype(data)(windowData);
        
        for (StateEnum state = StateEnum::Base; state != StateEnum::Count; EnumIncrement(state))
        {
            SAFERELEASE(data->data[state].textLayout);
        }
    }
};
