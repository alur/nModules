/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.hpp
 *  The nModules Project
 *
 *  Represents a separator in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class SeparatorItem : public PopupItem {
public:
    explicit SeparatorItem(Drawable* parent);
    virtual ~SeparatorItem();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    int GetDesiredWidth(int maxWidth);

private:
    DrawableWindow::STATE hoverState;
};
