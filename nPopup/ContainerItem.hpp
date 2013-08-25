/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContainerItem.hpp
 *  The nModules Project
 *
 *  Represents a popup item which can hold child windows.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"
#include "../nShared/StateRender.hpp"

class ContainerItem : public PopupItem
{
public:
    enum class State
    {
        Base = 0,
        Hover,
        Count
    };

public:
    explicit ContainerItem(Drawable* parent, LPCTSTR prefix);
    virtual ~ContainerItem();
    
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);
    int GetDesiredWidth(int maxWidth);

private:
    StateRender<State> mStateRender;
};
