/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowBangs.h
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Window.hpp"

namespace WindowBangs
{
    void Register(LPCTSTR prefix, Window* (*windowFinder)(LPCTSTR));
    void UnRegister(LPCTSTR prefix);

    // Clickevent related
    void On(HWND, LPCTSTR);
    void Off(HWND, LPCTSTR);

    //
    void Hide(HWND, LPCTSTR);
    void Show(HWND, LPCTSTR);
    void Toggle(HWND, LPCTSTR);
    void Move(HWND, LPCTSTR);
    void Size(HWND, LPCTSTR);
    void Position(HWND, LPCTSTR);
    void SetAlwaysOnTop(HWND, LPCTSTR);
    void SetClickThrough(HWND, LPCTSTR);
    void SetParent(HWND, LPCTSTR);
    void SetText(HWND, LPCTSTR);

    struct BangItem
    {
        LPCTSTR name;
        LiteStep::BANGCOMMANDPROC command;
    };
}
