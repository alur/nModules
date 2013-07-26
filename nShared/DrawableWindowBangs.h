/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowBangs.h
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "DrawableWindow.hpp"

namespace DrawableWindowBangs {
    void _Register(LPCSTR prefix, DrawableWindow* (*drawableFinder)(LPCSTR));
    void _UnRegister(LPCSTR prefix);

    // Clickevent related
    void On(HWND, LPCSTR);
    void Off(HWND, LPCSTR);

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
