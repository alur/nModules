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
#include "Tooltip.hpp"

class Balloon : public Drawable {
public:
    explicit Balloon(LPCSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler);
    virtual ~Balloon();

    void Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID drawableWindow);

private:
    UINT clickedMessage;
    MessageHandler* callbackHandler;

    DrawableWindow* titleWindow;
    DrawableWindow* closeBtnWindow;

    Settings* titleSettings;
    Settings* closeBtnSettings;
};
