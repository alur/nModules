/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.hpp
 *  The nModules Project
 *
 *  A general tooltip window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "Drawable.hpp"
#include "DrawableWindow.hpp"

class Tooltip : public Drawable {
public:
    explicit Tooltip(LPCSTR prefix);
    virtual ~Tooltip();

    void Show(LPCWSTR text, int x, int y, int duration);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
};
