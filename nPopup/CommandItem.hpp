/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.hpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class CommandItem : public PopupItem
{
public:
    enum class State
    {
        Base = 0,
        Hover,
        Count
    };

public:
    explicit CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, LPCTSTR customIcon = nullptr);
    explicit CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, HICON customIcon);
    virtual ~CommandItem();

private:
    explicit CommandItem(LPCTSTR title, LPCTSTR command, Drawable* parent);
    
public:
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

public:
    int GetDesiredWidth(int maxWidth);

private:
    LPCTSTR title;
    LPCTSTR command;
};
