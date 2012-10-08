/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayTip.hpp
 *  The nModules Project
 *
 *  Declaration of the TrayTip class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"

class TrayTip : public Drawable {
public:
    explicit TrayTip(LPCSTR pszPrefix);
    virtual ~TrayTip();

    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Show(LPCWSTR text);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    HWND GetHWND();
};
