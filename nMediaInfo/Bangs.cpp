/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Bangs.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"

#define IPC_ISPLAYING 104

#define WINAMP_BUTTON_PREVIOUS 40044
#define WINAMP_BUTTON_PLAY 40045
#define WINAMP_BUTTON_PAUSE 40046
#define WINAMP_BUTTON_STOP 40047
#define WINAMP_BUTTON_NEXT 40048

namespace Bangs
{
    struct BangItem
    {
        BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC handler)
        {
            this->name = name;
            this->handler = handler;
        }

        LPCTSTR name;
        LiteStep::BANGCOMMANDPROC handler;
    };

    static const BangItem bangMap[] =
    {
        // Skips to the previous track
        BangItem(L"!MediaPrevious", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                PostMessageW(WA2Window, WM_COMMAND, WINAMP_BUTTON_PREVIOUS, 0);
            }
        }),

        // Starts playing
        BangItem(L"!MediaPlay", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                PostMessageW(WA2Window, WM_COMMAND, WINAMP_BUTTON_PLAY, 0);
            }
        }),
        
        // Pauses the current track
        BangItem(L"!MediaPause", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                PostMessageW(WA2Window, WM_COMMAND, WINAMP_BUTTON_PAUSE, 0);
            }
        }),

        // Pauses the current track if currently playing. Plays if stoped.
        BangItem(L"!MediaPlayPause", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                SendMessageCallbackW(WA2Window, WM_USER, 0, IPC_ISPLAYING, (SENDASYNCPROC) [] (HWND hwnd, UINT, ULONG_PTR, LRESULT result) {
                    PostMessageW(hwnd, WM_COMMAND, result == 0 ? WINAMP_BUTTON_PLAY : WINAMP_BUTTON_PAUSE, 0);
                }, NULL);
            }
        }),

        // Stops playing.
        BangItem(L"!MediaStop", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                PostMessageW(WA2Window, WM_COMMAND, WINAMP_BUTTON_STOP, 0);
            }
        }),

        // Skips to the next track.
        BangItem(L"!MediaNext", [] (HWND, LPCTSTR)
        {
            HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
            if (WA2Window != nullptr)
            {
                PostMessageW(WA2Window, WM_COMMAND, WINAMP_BUTTON_NEXT, 0);
            }
        })
    };
    

    /// <summary>
    /// Registers bangs.
    /// </summary>
    void _Register()
    {
        for (const BangItem &bangItem : bangMap)
        {
            LiteStep::AddBangCommand(bangItem.name, bangItem.handler);
        }
    }


    /// <summary>
    /// Unregisters bangs.
    /// </summary>
    void _Unregister()
    {
        for (const BangItem &bangItem : bangMap)
        {
            LiteStep::RemoveBangCommand(bangItem.name);
        }
    }
}
