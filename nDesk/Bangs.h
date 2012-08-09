/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   Bangs.h															July, 2012
*   The nModules Project
*
*   Function declarations for Bangs.cpp
*      
*													             Erik Welander
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef BANGS_H
#define BANGS_H

namespace Bangs
{
    void _Register();
    void _Unregister();

    // Workarea related
    void SetWorkArea(HWND, LPCSTR);

    // Clickevent related
    void On(HWND, LPCSTR);
    void Off(HWND, LPCSTR);

    // Transition Related
    void SetTransitionDuration(HWND, LPCSTR);
    void SetTransitionFrameInterval(HWND, LPCSTR);
    void SetTransitionSquareSize(HWND, LPCSTR);
    void SetTransitionEffect(HWND, LPCSTR);

    void SetInvalidateAllOnUpdate(HWND, LPCSTR);

    typedef struct BangItem
    {
        char szName[54];
        BANGCOMMANDPROC pCommand;
    } BangItem;
}

#endif /* BANGS_H */