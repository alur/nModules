/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.hpp
 *  The nModules Project
 *
 *  A general balloon popup window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "DrawableWindow.hpp"

class Balloon : public Drawable
{
public:
    explicit Balloon(LPCTSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler);
    virtual ~Balloon();

    void Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID drawableWindow);

private:
    UINT mClickedMessage;
    MessageHandler* mCallbackHandler;

    DrawableWindow* mTitleWindow;
    DrawableWindow* mCloseBtnWindow;

    Settings* mTitleSettings;
    Settings* mCloseBtnSettings;
};
