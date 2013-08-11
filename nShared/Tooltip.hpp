/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.hpp
 *  The nModules Project
 *
 *  A general tooltip window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "Drawable.hpp"
#include "Window.hpp"

class Tooltip : public Drawable {
public:
    explicit Tooltip(LPCTSTR prefix, Settings* parentSettings);
    virtual ~Tooltip();

    void Show(LPCWSTR text, LPRECT position);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID);

private:
    int maxWidth;

    int maxHeight;
};
