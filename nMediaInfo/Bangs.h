/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Bangs.h
*  The nModules Project
*
*  Function declarations for Bangs.cpp
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Bangs {
    void _Register();
    void _Unregister();

    // Transition Related
    void MediaPrevious(HWND, LPCSTR);
    void MediaPlay(HWND, LPCSTR);
    void MediaPause(HWND, LPCSTR);
    void MediaPlayPause(HWND, LPCSTR);
    void MediaStop(HWND, LPCSTR);
    void MediaNext(HWND, LPCSTR);

    struct BangItem {
        char szName[54];
        LiteStep::BANGCOMMANDPROC pCommand;
    };
}
