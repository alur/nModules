/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "Bangs.h"
#include "../nCoreCom/Core.h"

#define IPC_ISPLAYING 104

namespace Bangs {
    BangItem BangMap[] = {
        { "MediaPrevious",      MediaPrevious       },
        { "MediaPlay",          MediaPlay           },
        { "MediaPause",         MediaPause          },
        { "MediaPlayPause",     MediaPlayPause      },
        { "MediaStop",          MediaStop           },
        { "MediaNext",          MediaNext           }
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
    char szBangName[64];
    for (BangItem &bangItem : BangMap) {
        StringCchPrintfA(szBangName, sizeof(szBangName), "!%s", bangItem.szName);
        LiteStep::AddBangCommand(szBangName, bangItem.pCommand);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
    char szBangName[64];
    for (BangItem &bangItem : BangMap) {
        StringCchPrintfA(szBangName, sizeof(szBangName), "!%s", bangItem.szName);
        LiteStep::RemoveBangCommand(szBangName);
    }
}


/// <summary>
/// Skips to the previous track
/// </summary>
void Bangs::MediaPrevious(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        PostMessageW(WA2Window, WM_COMMAND, 40044, 0);
    }
}


/// <summary>
/// Starts playing.
/// </summary>
void Bangs::MediaPlay(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        PostMessageW(WA2Window, WM_COMMAND, 40045, 0);
    }
}


/// <summary>
/// Pauses the current track.
/// </summary>
void Bangs::MediaPause(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        PostMessageW(WA2Window, WM_COMMAND, 40046, 0);
    }
}


/// <summary>
/// Pauses the current track if currently playing. Plays if stoped.
/// </summary>
void Bangs::MediaPlayPause(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        SendMessageCallbackW(WA2Window, WM_USER, 0, IPC_ISPLAYING, [] (HWND hwnd, UINT, ULONG_PTR, LRESULT result) {
            if (result == 0) { // Stoped, play
                PostMessageW(hwnd, WM_COMMAND, 40045, 0);
            }
            else { // Pause
                PostMessageW(hwnd, WM_COMMAND, 40046, 0);
            }
        }, NULL);
    }
}


/// <summary>
/// Stops playing.
/// </summary>
void Bangs::MediaStop(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        PostMessageW(WA2Window, WM_COMMAND, 40047, 0);
    }
}


/// <summary>
/// Skips to the next track.
/// </summary>
void Bangs::MediaNext(HWND, LPCSTR) {
    HWND WA2Window = FindWindowW(L"Winamp v1.x", nullptr);
    if (WA2Window != nullptr) {
        PostMessageW(WA2Window, WM_COMMAND, 40048, 0);
    }
}
