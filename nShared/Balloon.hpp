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

class Balloon : protected Tooltip {
public:
    explicit Balloon(LPCSTR prefix);
    virtual ~Balloon();

    void Show(LPCWSTR text, int x, int y, int duration);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
};
