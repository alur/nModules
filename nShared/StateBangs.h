/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateBangs.h
 *  The nModules Project
 *
 *  Bangs for states.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"

namespace StateBangs {
    void _Register(LPCSTR prefix, DrawableWindow::STATE (*stateFinder)(LPCSTR, LPCSTR));
    void _UnRegister(LPCSTR prefix);

    // 
    void Hide(HWND, LPCSTR);
    void Show(HWND, LPCSTR);
    void Move(HWND, LPCSTR);
    void Size(HWND, LPCSTR);
    void Position(HWND, LPCSTR);
    void SetAlwaysOnTop(HWND, LPCSTR);
    void SetClickThrough(HWND, LPCSTR);
    void SetParent(HWND, LPCSTR);
    void SetText(HWND, LPCSTR);

    struct BangItem {
        LPCSTR name;
        LiteStep::BANGCOMMANDPROC command;
    };
}
