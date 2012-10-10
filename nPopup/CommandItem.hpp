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
    explicit CommandItem(Drawable* parent, LPCSTR title, LPCSTR command, LPCSTR customIcon = NULL);
    explicit CommandItem(Drawable* parent, LPCSTR title, LPCSTR command, HICON customIcon);
    virtual ~CommandItem();
    
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    void Init(LPCSTR title, LPCSTR command);

    LPCSTR title;
    LPCSTR command;

    DrawableWindow::STATE hoverState;
};
