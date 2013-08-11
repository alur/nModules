/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.hpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class CommandItem : public PopupItem {
public:
    explicit CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, LPCTSTR customIcon = nullptr);
    explicit CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, HICON customIcon);
    virtual ~CommandItem();
    
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);
    int GetDesiredWidth(int maxWidth);

private:
    void Init(LPCTSTR title, LPCTSTR command);

    LPCTSTR title;
    LPCTSTR command;

    Window::STATE hoverState;
};
