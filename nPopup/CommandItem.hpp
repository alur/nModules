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
    explicit CommandItem(LPCSTR title, LPCSTR command, LPCSTR customIcon = NULL);
    virtual ~CommandItem();
    
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void Init(Settings* parentSettings, DrawableWindow* parentWindow);

private:
    LPCSTR title;
    LPCSTR command;
};
