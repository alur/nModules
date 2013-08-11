/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContainerItem.hpp
 *  The nModules Project
 *
 *  Represents a popup item which can hold child windows.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class ContainerItem : public PopupItem {
public:
    explicit ContainerItem(Drawable* parent, LPCTSTR prefix);
    virtual ~ContainerItem();
    
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);
    int GetDesiredWidth(int maxWidth);

private:
    Window::STATE hoverState;
};
