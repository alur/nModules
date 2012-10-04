/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.cpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../headers/lsapi.h"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/IDrawableMessageHandler.hpp"

class PopupItem : public IDrawableMessageHandler {
public:
    virtual ~PopupItem();
    void Position(int x, int y);
    virtual LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM) = 0;

protected:
    DrawableWindow* window;
    Settings* settings;
};
