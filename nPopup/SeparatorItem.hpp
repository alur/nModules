/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.hpp
 *  The nModules Project
 *
 *  Represents a separator in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class SeparatorItem : public PopupItem
{
public:
    enum class State
    {
        Base = 0,
        Hover,
        Count
    };

public:
    explicit SeparatorItem(Drawable* parent);
    virtual ~SeparatorItem();

public:
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

public:
    int GetDesiredWidth(int maxWidth);
};
