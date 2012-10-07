/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.cpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../headers/lsapi.h"
#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/MessageHandler.hpp"

class PopupItem : public Drawable {
public:
    explicit PopupItem(Drawable* parent);
    virtual ~PopupItem();
    void Position(int x, int y);
    virtual LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM) = 0;
    int GetHeight();
};
