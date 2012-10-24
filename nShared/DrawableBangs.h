/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableBangs.h
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"

namespace DrawableBangs {
    void _Register(LPCSTR prefix, Drawable* (*drawableFinder)(LPCSTR));
    void _UnRegister(LPCSTR prefix);

    // Clickevent related
    void On(HWND, LPCSTR);
    void Off(HWND, LPCSTR);

    // 
    void Hide(HWND, LPCSTR);
    void Show(HWND, LPCSTR);

    typedef struct {
        char szName[64];
        LiteStep::BANGCOMMANDPROC pCommand;
    } BangItem;
}
