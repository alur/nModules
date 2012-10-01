/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IDrawableMessageHandler.h
 *  The nModules Project
 *
 *  Interface for a class which can accept message for a DrawableWindow.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class IDrawableMessageHandler {
public:
    virtual LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};
