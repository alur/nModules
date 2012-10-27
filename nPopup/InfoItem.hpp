/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InfoItem.hpp
 *  The nModules Project
 *
 *  Represents a line of info.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class InfoItem : public PopupItem {
public:
    explicit InfoItem(Drawable* parent, LPCSTR title, LPCSTR customIcon = NULL);
    virtual ~InfoItem();
    
    int GetDesiredWidth(int maxWidth);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    LPCSTR title;

    DrawableWindow::STATE hoverState;
};
